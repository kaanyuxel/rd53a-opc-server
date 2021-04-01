#include "RD53Emulator/Emulator.h"
#include "RD53Emulator/Encoder.h"
#include "RD53Emulator/Decoder.h"
#include "RD53Emulator/RdReg.h"
#include "RD53Emulator/Tools.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>

using namespace std;
using namespace RD53A;

Emulator::Emulator(uint32_t chipid,uint32_t mode){
  m_decoder = new Decoder();
  m_encoder = new Encoder();
  m_config = new Configuration();
  m_matrix = new Matrix();
  m_verbose = 0;
  m_randomThresholds = false;
  m_isInitialized = false;
  m_register_index = 0;
  m_outmode = mode;
  m_chipid = chipid;
  m_ndf=0;
  m_sigmaNoiseDistribution = 150.;
  m_pixelNoiseDistribution = std::normal_distribution<double>(0., m_sigmaNoiseDistribution); // a pixel noise of 40 electrons
  m_th_syn = 0;
  m_th_lin = 0;
  m_th_diff = 0;
}

Emulator::~Emulator(){
  delete m_decoder;
  delete m_encoder;
  delete m_config;
  delete m_matrix;
  for(uint32_t col=0; col<400; col++){
    delete m_thresholds[col];
  }
  delete[] m_thresholds;
}

void Emulator::SetChipID(uint32_t chipid){
   m_chipid=chipid;
}

uint32_t Emulator::GetChipID(){
  return m_chipid;
}

void Emulator::HandleCommand(uint8_t *recv_data, uint32_t recv_size){

  if(recv_size==0){return;}
  if(m_verbose) cout << "Emulator::HandleCommand received commands size : " << recv_size << endl;

  m_encoder->Clear();
  m_encoder->SetBytes(recv_data, recv_size);
  if(m_verbose) cout << "Emulator::HandleCommand Byte stream: " << m_encoder->GetByteString() << endl;
  m_encoder->Decode();

  for(uint32_t i=0;i<m_encoder->GetCommands().size();i++){
    if(m_verbose>1) cout << "Emulator::HandleCommand " << setw(2) << i << " " << m_encoder->GetCommands()[i]->ToString() << endl;
    //Actually put in a queue and process it later
    m_cmds.push_back(m_encoder->GetCommands()[i]->Clone());
  }

}

void Emulator::ProcessQueue(){

  m_decoder->Clear();

  //uint32_t nfs = m_config->GetField(Configuration::MON_FRAME_SKIP)->GetValue();
  uint32_t nfs = 10;

  m_ndf++;
  if(m_ndf%nfs==0 and m_outmode!=OUTPUT_DATA){AddServiceFrame();}

  while(!m_cmds.empty()){

    Command * cmd = m_cmds.front();
  
    if(m_verbose) cout << "Emulator::ProcessQueue Command: " << cmd->ToString() << endl;

    if(cmd->GetType()==Command::RDREG){
      RdReg* rd_reg=dynamic_cast<RdReg*>(cmd);
      if(rd_reg->GetAddress() == 136){
         uint32_t data = CreateRandomADCData();
         m_config->SetRegister(rd_reg->GetAddress(), data);
         auto end = std::chrono::system_clock::now();
         cout << "----> Created Data : " << std::chrono::system_clock::to_time_t(end) << " " << data << endl;
      }
      m_read_reqs.push(rd_reg->GetAddress());
    }
    else if(cmd->GetType()==Command::WRREG){
      WrReg*wrreg=dynamic_cast<WrReg*>(cmd);
      if(wrreg->GetType()==1){continue;}
      //Read ADC
      if(wrreg->GetAddress() == 44 && wrreg->GetValue() == 8){
        m_config->SetRegister(wrreg->GetAddress(), wrreg->GetValue());
      }
      //PIXEL PORTAL
      if(wrreg->GetAddress()==0){
        m_matrix->SetPair(m_config->GetField(Configuration::REGION_COL)->GetValue(),
            m_config->GetField(Configuration::REGION_ROW)->GetValue(),
            wrreg->GetValue());
      }else{
        m_config->SetRegister(wrreg->GetAddress(),wrreg->GetValue());      
      }
    }
    else if(cmd->GetType()==Command::TRIGGER){
      if(m_verbose) cout << "Emulator::ProcessQueue Process Trigger" << endl;
      // before the first trigger (after all configurations), initialize thresholds
      if(!m_isInitialized){
        InitThresholds();
        m_isInitialized = true;
        return;
      }

      if(m_th_syn!=m_config->GetField(Configuration::VTH_SYNC)->GetValue() or m_th_lin!=m_config->GetField(Configuration::VTH_LIN)->GetValue() or m_th_diff!=m_config->GetField(Configuration::VTH1_DIFF)->GetValue()){
        InitThresholds();
      } //This is needed in order to perform a threshold tuning, which consists in a loop of analog scan inside a loop over threshold values. Whitout this if condition, the InitThreshold function is called inside the first loop only. 

      //always send back a header
      DataFrame * df = new DataFrame();
      df->SetFormat(DataFrame::SYN_HDR);
      df->SetHeader(1,0,((Trigger*)cmd)->GetTag(),0);
      m_decoder->AddFrame(df);
      m_ndf++;
      if(m_ndf%nfs==0 and m_outmode!=OUTPUT_DATA){AddServiceFrame();}

      //Loop over the matrix
      uint32_t reg, off, DAC; 
      for(uint32_t ccol=0;ccol<50;ccol++){
        if     (ccol>= 0 and ccol<16){ reg=Configuration::EN_CORE_COL_SYNC;   off= 0; DAC=m_config->GetField(Configuration::IBIAS_KRUM_SYNC)->GetValue();}
        else if(ccol>=16 and ccol<32){ reg=Configuration::EN_CORE_COL_LIN_1;  off=16; DAC=m_config->GetField(Configuration::KRUM_CURR_LIN)->GetValue();}
        else if(ccol==32)            { reg=Configuration::EN_CORE_COL_LIN_2;  off=32; DAC=m_config->GetField(Configuration::KRUM_CURR_LIN)->GetValue();}
        else if(ccol>=33 and ccol<49){ reg=Configuration::EN_CORE_COL_DIFF_1; off=33; DAC=m_config->GetField(Configuration::VFF_DIFF)->GetValue();}
        else if(ccol==49)            { reg=Configuration::EN_CORE_COL_DIFF_2; off=49; DAC=m_config->GetField(Configuration::VFF_DIFF)->GetValue();}
        bool enable = (m_config->GetField(reg)->GetValue() & (1<<(ccol-off)));

        if(enable){
          //loop over quad columns
          for(uint32_t qcol=ccol*2; qcol<(ccol+1)*2; qcol++){
            //loop over rows
            for(uint32_t row=0; row<192; row++){
              //loop over columns
              uint32_t tot[4]={0,0,0,0};
              bool hashit=false;
              for(uint32_t i=0;i<4;i++){
                //check enabled
                if(m_matrix->GetPixel(qcol*4+i,row)->GetEnable()){
		  if(m_config->GetField(Configuration::INJ_MODE_DIG)->GetValue()==1) {
		      hashit=true;
		      tot[i]=4; //don't remove, otherwise the digital scan won't work
		    }
		  else {
		    unsigned int vcal = m_config->GetField(Configuration::VCAL_HIGH)->GetValue() - m_config->GetField(Configuration::VCAL_MED)->GetValue();
		    double chargeInj = Tools::injToCharge(vcal);
		    double pixelNoise = 0.;
		    m_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
		    if(m_pixelNoise) pixelNoise = m_pixelNoiseDistribution(m_generator);
		    uint32_t TDAC = m_matrix->GetPixel(qcol*4+i,row)->GetTDAC();
		    int thrAdj = TDAC*10 - 80; //electrons, from -80 to 70
		    double chargeTh = m_thresholds[qcol*4+i][row] + pixelNoise + thrAdj;

		    //above threshold
		    if(chargeInj > chargeTh){
		      hashit=true;
		      int charge = chargeInj - chargeTh;
		      //	      tot[i]=charge/1600;
		      tot[i]= Tools::chargeToToT(DAC, charge, ccol); 
		      if(tot[i]>15) tot[i]=15; 
		    }
		    else tot[i]=0;
		     }
		}
              }

              //any hit
              if(hashit){
                DataFrame * df = new DataFrame();
                df->SetFormat(DataFrame::SYN_HIT);
                df->SetHit(1,qcol,row,tot);
                m_decoder->AddFrame(df);
                m_ndf++;
                if(m_ndf%nfs==0 and m_outmode!=OUTPUT_DATA){AddServiceFrame();}
              }

            }
          }
        }
	
      }

    }
    m_cmds.pop_front();
    delete cmd;
  }

  m_decoder->Encode();

  if(m_verbose > 1){
    cout << "Emulator::ProcessQueue" << endl;
    for(uint32_t i=0;i<m_decoder->GetFrames().size();i++){
      cout << setw(2) << i << " " << m_decoder->GetFrames()[i]->ToString() << endl;
    }
  }

}

void Emulator::AddServiceFrame(){
  if(m_verbose > 1){
    cout << "Emulator::AddServiceFrame" << endl;
  }
  //Add 1 register frame that can contain 2 addresses
  RegisterFrame *reg = new RegisterFrame();
  uint32_t sz=m_read_reqs.size();
  cout << " ReadRegister Size : " << sz << endl;
  for(uint32_t i=0;i<2;i++){
    uint32_t addr;
    if(sz>i){
      addr = m_read_reqs.front();
      m_read_reqs.pop();
      reg->SetAuto(i,0);
      cout << i << " " << addr << endl;
    }else{
      addr = GetNextRegister();
      reg->SetAuto(i,1);
    }
    auto end = std::chrono::system_clock::now();
    cout << i << " Emulator sent time : "<< std::chrono::system_clock::to_time_t(end) << " " << addr << " " << m_config->GetRegister(addr) << endl;
    reg->SetRegister(i,addr,m_config->GetRegister(addr));
  }
  m_decoder->AddFrame(reg);
  m_ndf=0;
}

void Emulator::SetVerbose(int lvl){
   m_verbose = lvl;
}

void Emulator::SetRandomThresholds(bool enable){
   m_randomThresholds = enable;
}

void Emulator::SetPixelNoise(bool enable){
   m_pixelNoise = enable;
}

void Emulator::Clear(){
  m_decoder->Clear();
}

void Emulator::InitThresholds(){

  // threshold dispersions for random threshold initialization (sync, lin, diff)
  double vthDispersion[3] = {30., 100., 240.}; //30., 400., 240. 

  // allocating threshold matrix
  m_thresholds = new double*[400];
  for(uint32_t col=0; col<400; col++){
    m_thresholds[col] = new double[192];
  }

  // setting individual pixel thresholds
  uint32_t vth, vthIndex;
  for(uint32_t ccol=0;ccol<50;ccol++){

    if     (ccol>= 0 and ccol<16){vth=m_config->GetField(Configuration::VTH_SYNC)->GetValue(); vthIndex=0; m_th_syn=vth;}
    else if(ccol>=16 and ccol<32){vth=m_config->GetField(Configuration::VTH_LIN)->GetValue(); vthIndex=1; m_th_lin=vth;}
    else if(ccol==32)            {vth=m_config->GetField(Configuration::VTH_LIN)->GetValue(); vthIndex=1; m_th_lin=vth;}
    else if(ccol>=33 and ccol<49){vth=m_config->GetField(Configuration::VTH1_DIFF)->GetValue(); vthIndex=2; m_th_diff=vth;}
    else if(ccol==49)            {vth=m_config->GetField(Configuration::VTH1_DIFF)->GetValue(); vthIndex=2; m_th_diff=vth;}

    for(uint32_t qcol=ccol*2; qcol<(ccol+1)*2; qcol++){
      for(uint32_t row=0; row<192; row++){
	for(uint32_t i=0;i<4;i++){
	  if(m_randomThresholds){
	    std::normal_distribution<double> distribution(Tools::thrToCharge(vth, ccol), vthDispersion[vthIndex]);
	    double rdmThreshold = distribution(m_generator);
	    if(rdmThreshold < 0.) rdmThreshold = 0.;
	    m_thresholds[qcol*4+i][row] = rdmThreshold;
	  }
	  else m_thresholds[qcol*4+i][row] = Tools::thrToCharge(vth, ccol);
	}
      }
    }
  }

  if(m_randomThresholds or m_pixelNoise){
  std::ofstream logFileEmulator;
  logFileEmulator.open("logFileEmulator.txt");
  logFileEmulator << "m_randomThresholds: " << std::boolalpha << m_randomThresholds << ", sigma sync: " << vthDispersion[0] << ", sigma linear: " << vthDispersion[1] << ", sigma differential: " << vthDispersion[2] << endl;
  logFileEmulator << "m_pixelNoise: " << std::boolalpha << m_pixelNoise << ", sigma: " << m_sigmaNoiseDistribution << endl;
  logFileEmulator.close();
  }

  /*
   std::ofstream trueThrFile("trueThreshold.txt");
   for(uint32_t col=0; col<400; col++){
     for(uint32_t row=0; row<192; row++){
       trueThrFile << (int)m_thresholds[col][row] << " ";
     } 
     trueThrFile << endl;
   }
   trueThrFile.close();
  */

  // printing values
  if(m_verbose){
  cout << "Emulator::InitThresholds() Initialized threshold values:" << endl;
    for(uint32_t col=0; col<400; col+=10){
      cout << "col " << setw(3) << col << ":";
      for(uint32_t row=0; row<192; row+=10){
	cout << setw(5) << (int)m_thresholds[col][row];
      } 
      cout << endl;
    }
  }
}

uint32_t Emulator::CreateRandomADCData(){
   return (rand() % 5000) + 1;
}

uint8_t *Emulator::GetBytes(){
  return m_decoder->GetBytes();
}

uint32_t Emulator::GetLength(){
  return m_decoder->GetLength();
}

uint32_t Emulator::GetNextRegister(){
  uint32_t addr=m_register_index;
  m_register_index++;
  if(m_register_index>=0x1FF){m_register_index=0;}
  return addr;
}

void Emulator::SetOutputMode(uint32_t mode){
  m_outmode = mode;
}
