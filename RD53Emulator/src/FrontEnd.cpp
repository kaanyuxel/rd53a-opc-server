#include "RD53Emulator/FrontEnd.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace RD53A;

FrontEnd::FrontEnd(){
  m_decoder = new Decoder();
  m_encoder = new Encoder();
  m_config  = new Configuration();
  m_matrix  = new Matrix();
  m_verbose = 1;
  m_chipid = 0;
  m_name = "RD53A";
  m_active=true;
  for(uint32_t i=0;i<4;i++){
    m_ntcs.push_back(new TemperatureSensor());
    m_bjts.push_back(new RadiationSensor());
  }
}

FrontEnd::~FrontEnd(){
  delete m_decoder;
  delete m_encoder;
  delete m_config;
  delete m_matrix;
  for(uint32_t i=0;i<4;i++){
    delete m_ntcs[i];
    delete m_bjts[i];
  }
}

void FrontEnd::SetVerbose(bool enable){
  m_verbose = enable;
}

void FrontEnd::SetChipID(uint32_t chipid){
  m_chipid=chipid;
}

uint32_t FrontEnd::GetChipID(){
  return m_chipid;
}

void FrontEnd::SetName(string name){
  m_name=name;
}

string FrontEnd::GetName(){
  return m_name;
}

Configuration * FrontEnd::GetConfig(){
  return m_config;
}

vector<Command*> & FrontEnd::GetCommands(){
  return m_encoder->GetCommands();
}

void FrontEnd::Clear(){
  m_encoder->Clear();
}

bool FrontEnd::IsActive(){
  return m_active;
}

void FrontEnd::SetActive(bool active){
  m_active=active;
}

uint32_t FrontEnd::GetGlobalThreshold(uint32_t type){
  switch (type){
  case Pixel::Lin: return m_config->GetField(Configuration::VTH_LIN)->GetValue();
  case Pixel::Diff: return m_config->GetField(Configuration::VTH1_DIFF)->GetValue();
  case Pixel::Sync: return m_config->GetField(Configuration::VTH_SYNC)->GetValue();
  }
  return 0;
}

void FrontEnd::SetGlobalThreshold(uint32_t type, uint32_t threshold){
  switch (type){
  case Pixel::Lin: return m_config->GetField(Configuration::VTH_LIN)->SetValue(threshold);
  case Pixel::Diff: return m_config->GetField(Configuration::VTH1_DIFF)->SetValue(threshold);
  case Pixel::Sync: return m_config->GetField(Configuration::VTH_SYNC)->SetValue(threshold);
  }
}

bool FrontEnd::GetPixelEnable(uint32_t col, uint32_t row){
  return m_matrix->GetPixel(col,row)->GetEnable();
}

void FrontEnd::SetPixelEnable(uint32_t col, uint32_t row, bool enable){
  return m_matrix->GetPixel(col,row)->SetEnable(enable);
}

bool FrontEnd::GetPixelInject(uint32_t col, uint32_t row){
  return m_matrix->GetPixel(col,row)->GetInject();
}

void FrontEnd::SetPixelInject(uint32_t col, uint32_t row, bool enable){
  return m_matrix->GetPixel(col,row)->SetInject(enable);
}

bool FrontEnd::GetPixelHitbus(uint32_t col, uint32_t row){
  return m_matrix->GetPixel(col,row)->GetHitbus();
}

void FrontEnd::SetPixelHitbus(uint32_t col, uint32_t row, bool enable){
  return m_matrix->GetPixel(col,row)->SetHitbus(enable);
}

uint32_t FrontEnd::GetPixelThreshold(uint32_t col, uint32_t row){
  return m_matrix->GetPixel(col,row)->GetTDAC();
}

void FrontEnd::SetPixelThreshold(uint32_t col, uint32_t row, uint32_t threshold){
  return m_matrix->GetPixel(col,row)->SetTDAC(threshold);
}

bool FrontEnd::GetPixelGain(uint32_t col, uint32_t row){
  return m_matrix->GetPixel(col,row)->GetGain();
}

void FrontEnd::SetPixelGain(uint32_t col, uint32_t row, bool enable){
  return m_matrix->GetPixel(col,row)->SetGain(enable);
}

void FrontEnd::SetGlobalConfig(map<string,uint32_t> config){
  m_config->SetRegisters(config);
}

map<string,uint32_t> FrontEnd::GetGlobalConfig(){
  return m_config->GetRegisters();
}

TemperatureSensor * FrontEnd::GetTemperatureSensor(uint32_t index){
  return m_ntcs[index];
}

RadiationSensor * FrontEnd::GetRadiationSensor(uint32_t index){
  return m_bjts[index];
}

void FrontEnd::WriteGlobal(){
  for(auto addr : m_config->GetUpdatedRegisters()){
    m_encoder->AddCommand(new WrReg(m_chipid,addr,m_config->GetRegister(addr)));
  }
}

void FrontEnd::ReadGlobal(){
  for(uint32_t addr=0;addr<=137;addr++){
    m_encoder->AddCommand(new RdReg(m_chipid,addr));
  }
}

void FrontEnd::WritePixels(){
  for(uint32_t dcol=0;dcol<200;dcol++){
    for(uint32_t row=0;row<192;row++){
      WritePixelPair(dcol,row);
    }
  }
}

void FrontEnd::WritePixelPair(uint32_t double_col, uint32_t row){
  uint32_t value = m_matrix->GetPair(double_col,row);
  m_config->GetField(Configuration::REGION_COL)->SetValue(double_col);
  m_config->GetField(Configuration::REGION_ROW)->SetValue(row);
  m_config->GetField(Configuration::PIX_PORTAL)->SetValue(value);
  WriteGlobal();
}

void FrontEnd::ReadPixels(){
  for(uint32_t dcol=0;dcol<200;dcol++){
    for(uint32_t row=0;row<192;row++){
      ReadPixelPair(dcol,row);
    }
  }
}

void FrontEnd::ReadPixelPair(uint32_t double_col, uint32_t row){
  m_config->GetField(Configuration::REGION_COL)->SetValue(double_col);
  m_config->GetField(Configuration::REGION_ROW)->SetValue(row);
  WriteGlobal();
  m_encoder->AddCommand(new RdReg(m_chipid,0));
}

void FrontEnd::SetMask(uint32_t mask_mode, uint32_t mask_iter){
  if(m_verbose) std::cout << __PRETTY_FUNCTION__ << ": setting mask to mode " << mask_mode << " iter " << mask_iter << std::endl;
  for(unsigned col=0; col<400; col++) {
    for(unsigned row=0; row<192; row++) {
      //if (ignorePixel(col, row)) continue;
      unsigned core_row = row/8;
      unsigned serial = (core_row*64)+((col+(core_row%8))%8)*8+row%8;
      if(mask_mode == 0) m_matrix->GetPixel(col,row)->SetEnable(true); // all pixels enabled at once
      else{
        bool enable = ((serial%mask_mode) == mask_iter?true:false);
        m_matrix->GetPixel(col,row)->SetEnable(enable);
      } 
    }
  }
  WritePixels();
} 

void FrontEnd::SelectCoreColumn(uint32_t ccol, bool enable){
  uint32_t reg=0;
  uint32_t off=0;
  if     (ccol>= 0 and ccol<16){ reg=Configuration::EN_CORE_COL_SYNC;   off= 0; }
  else if(ccol>=16 and ccol<32){ reg=Configuration::EN_CORE_COL_LIN_1;  off=16; }
  else if(ccol==32)            { reg=Configuration::EN_CORE_COL_LIN_2;  off=32; }
  else if(ccol>=33 and ccol<49){ reg=Configuration::EN_CORE_COL_DIFF_1; off=33; }
  else if(ccol==49)            { reg=Configuration::EN_CORE_COL_DIFF_2; off=49; }
  uint32_t value = m_config->GetField(reg)->GetValue();
  if(enable) value |= (1<<(ccol-off));
  else       value &= ~(1<<(ccol-off));
  m_config->GetField(reg)->SetValue(value);
  WriteGlobal();
}

void FrontEnd::SetCoreColumn(uint32_t mode, uint32_t index, string frontend){
  uint32_t start=0;
  uint32_t stop=0;
  if(index>mode) return;
  //  uint32_t stp=50/mode; // EJS 2020-11-08: not convinced this is correct
  uint32_t stp=mode; // EJS 2020-11-08: this might be the correct way
  if(frontend.find("syn")!=string::npos){start=0; stop=16;}
  if(frontend.find("lin")!=string::npos){start=16; stop=33;}
  if(frontend.find("diff")!=string::npos){start=33; stop=50;}
  if(frontend.find("all")!=string::npos){start=0; stop=50;}
  for(uint32_t cc=0;cc<50;cc++){
    bool ccenable = ((index+cc)%stp==0?true:false); // EJS 2020-11-08
    if(cc>=start and cc<stop) SelectCoreColumn(cc,ccenable);
    else SelectCoreColumn(cc,false);
  }
}

void FrontEnd::InitAdc(){
  m_config->GetField(Configuration::ADC_TRIM)->SetValue(0);
  m_config->GetField(Configuration::BANDGAP_TRIM)->SetValue(0);
  m_config->GetField(Configuration::GLOBAL_PULSE_RT)->SetValue(8);
  WriteGlobal();
  m_encoder->AddCommand(new Pulse(m_chipid));
}

void FrontEnd::ReadSensor(uint32_t pos, bool read_radiation_sensor){
  if(pos==0){
    m_config->GetField(Configuration::SENSOR_BIAS_0)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_CURRENT_0)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_ENABLE_0)->SetValue(1);
    if(read_radiation_sensor == true)
       m_bjts[pos]->SetPower(true);
    else
      m_ntcs[pos]->SetPower(true);
  }else if(pos==1){
    m_config->GetField(Configuration::SENSOR_BIAS_1)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_CURRENT_1)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_ENABLE_1)->SetValue(1);
    if(read_radiation_sensor == true)
       m_bjts[pos]->SetPower(true);
    else
      m_ntcs[pos]->SetPower(true);
  }else if(pos==2){
    m_config->GetField(Configuration::SENSOR_BIAS_2)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_CURRENT_2)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_ENABLE_2)->SetValue(1);
    if(read_radiation_sensor == true)
       m_bjts[pos]->SetPower(true);
    else
      m_ntcs[pos]->SetPower(true);
  }else if(pos==3){
    m_config->GetField(Configuration::SENSOR_BIAS_3)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_CURRENT_3)->SetValue(1);
    m_config->GetField(Configuration::SENSOR_ENABLE_3)->SetValue(1);
    if(read_radiation_sensor == true)
       m_bjts[pos]->SetPower(true);
    else
      m_ntcs[pos]->SetPower(true);
  }
  m_config->GetField(Configuration::GLOBAL_PULSE_RT)->SetValue(6);
  WriteGlobal();
  m_encoder->AddCommand(new Pulse(m_chipid));
  m_config->GetField(Configuration::GLOBAL_PULSE_RT)->SetValue(3);
  WriteGlobal();
  m_encoder->AddCommand(new Pulse(m_chipid));
  if(pos==0){
	if(read_radiation_sensor == true)
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(4);
	else
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(3);
  }else if(pos==1){
    if(read_radiation_sensor == true)
      m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(6);
	else
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(5);
  }else if(pos==2){
    if(read_radiation_sensor == true)
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(15);
    else
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(14);
  }else if(pos==3){
	if(read_radiation_sensor == true)
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(8);
	else
	  m_config->GetField(Configuration::MONITOR_VMON_MUX)->SetValue(7);
  }
  m_config->GetField(Configuration::GLOBAL_PULSE_RT)->SetValue(12);
  WriteGlobal();
  m_encoder->AddCommand(new Pulse(m_chipid));
  m_encoder->AddCommand(new RdReg(m_chipid,136));
}

void FrontEnd::Trigger(uint32_t delay){

  m_encoder->AddCommand(new Sync());
  m_encoder->AddCommand(new Noop());
  m_encoder->AddCommand(new Cal(7,0,0,1,0,0));
  for(uint32_t i=0;i<delay;i++){
    m_encoder->AddCommand(new Noop());
  }
  m_encoder->AddCommand(new RD53A::Trigger(1,0,0,0,0));

}

Hit * FrontEnd::GetHit(){
  Hit * tmp = NULL;
  m_mutex.lock();
  tmp = m_hits.front();
  m_mutex.unlock();
  return tmp;
}

bool FrontEnd::NextHit(){
  if (m_hits.empty()) return false;
  m_mutex.lock();
  Hit * hit = m_hits.front();
  m_hits.pop_front();
  delete hit;
  m_mutex.unlock();
  return (!m_hits.empty());
}

bool FrontEnd::HasHits(){
  return (not m_hits.empty());
}

void FrontEnd::HandleData(uint8_t *recv_data, uint32_t recv_size){

  if(m_verbose) cout << "FrontEnd::HandleData" <<endl;
  m_decoder->SetBytes(recv_data,recv_size);
  if(m_verbose){cout << "FrontEnd::HandleData Decode" << endl;}
  m_decoder->Decode(m_verbose);

  Hit hit;
  for(auto frame: m_decoder->GetFrames()){

    if(m_verbose) cout << "FrontEnd::HandleData() processing frame of type " << frame->GetType() << endl;

    if(frame->GetType()==Frame::REGISTER){

      RegisterFrame * reg=dynamic_cast<RegisterFrame*>(frame);
      if(m_verbose) cout << __PRETTY_FUNCTION__ << reg->ToString() << endl;
      if(reg->GetAuroraCode()!=0xCC){
        for(uint32_t i=0;i<2;i++){
          if(reg->GetAddress(i)==Configuration::PIX_PORTAL){
            uint32_t reg_col=m_config->GetField(Configuration::REGION_COL)->GetValue();
            uint32_t reg_row=m_config->GetField(Configuration::REGION_ROW)->GetValue();
            m_matrix->SetPair(reg_col,reg_row,reg->GetValue(i));
          }
          else if(reg->GetAddress(i)>Configuration::PIX_PORTAL and reg->GetAddress(i)<=0x1FF){
            m_config->SetRegister(reg->GetAddress(i),reg->GetValue(i));
          }
          if(reg->GetAddress(i) == 136){
        	for(int j=0; j < 4; j++){
        	  if(m_ntcs[j]->GetPower() == true && m_ntcs[j]->isUpdated() == false && reg->GetAuto(i) == 0){
        	    m_ntcs[j]->SetADC(reg->GetValue(i));
        	    m_ntcs[j]->Update(true);
        	  }
        	  else if(m_bjts[j]->GetPower() == true && m_bjts[j]->isUpdated() == false && reg->GetAuto(i) == 0){
        		m_bjts[j]->SetADC(reg->GetValue(i));
        		m_bjts[j]->Update(true);
          	  }
          	}
          }
        }
      }
    }else if(frame->GetType()==Frame::DATA){
      if(m_verbose) cout << "FrontEnd::HandleData: Data " << frame->ToString() << endl;
      DataFrame * dat=dynamic_cast<DataFrame*>(frame);
      if(dat->GetType()==DataFrame::SYN_HDR){
        hit.Update(dat->GetTID(1),dat->GetTTag(1),dat->GetBCID(1));
      }else if(dat->GetType()==DataFrame::HDR_HDR){
        hit.Update(dat->GetTID(0),dat->GetTTag(0),dat->GetBCID(0));
        hit.Update(dat->GetTID(1),dat->GetTTag(1),dat->GetBCID(1));
      }else if(dat->GetType()==DataFrame::HDR_HIT){
        hit.Update(dat->GetTID(0),dat->GetTTag(0),dat->GetBCID(0));
        for(uint32_t idx=0;idx<4;idx++){
          if(dat->GetTOT(1,idx)>0){
            Hit * nhit = hit.Clone();
            nhit->Set(dat->GetCol(1)+idx,dat->GetRow(1),dat->GetTOT(1,idx));
            m_mutex.lock();
            m_hits.push_back(nhit);
            m_mutex.unlock();
          }
        }
      }else if(dat->GetType()==DataFrame::SYN_HIT){
        for(uint32_t idx=0;idx<4;idx++){
          if(dat->GetTOT(1,idx)>0){
            Hit * nhit = hit.Clone();
            nhit->Set(dat->GetCol(1)+idx,dat->GetRow(1),dat->GetTOT(1,idx));
            m_mutex.lock();
            m_hits.push_back(nhit);
            m_mutex.unlock();
          }
        }
      }else if(dat->GetType()==DataFrame::HIT_HDR){
        for(uint32_t idx=0;idx<4;idx++){
          if(dat->GetTOT(0,idx)>0){
            Hit * nhit = hit.Clone();
            nhit->Set(dat->GetCol(0)+idx,dat->GetRow(0),dat->GetTOT(0,idx));
            m_mutex.lock();
            m_hits.push_back(nhit);
            m_mutex.unlock();
          }
        }
        hit.Update(dat->GetTID(1),dat->GetTTag(1),dat->GetBCID(1));
      }else if(dat->GetType()==DataFrame::HIT_HIT){
        for(uint32_t i=0;i<2;i++){
          for(uint32_t idx=0;idx<4;idx++){
            if(dat->GetTOT(i,idx)>0){
              Hit * nhit = hit.Clone();
              nhit->Set(dat->GetCol(i)+idx,dat->GetRow(i),dat->GetTOT(i,idx));
              m_mutex.lock();
              m_hits.push_back(nhit);
              m_mutex.unlock();
            }
          }
        }
      }

    }
  }
}

void FrontEnd::ProcessCommands(){
  if(m_verbose){
    for(auto cmd: m_encoder->GetCommands()){
      cout << __PRETTY_FUNCTION__ << "Command: " << cmd->ToString() << endl;
    }
  }
  m_encoder->Encode();
  if(m_verbose){
    cout << __PRETTY_FUNCTION__ << "Byte Stream: " << m_encoder->GetByteString() << endl;
  }
}

uint32_t FrontEnd::GetLength(){
  return m_encoder->GetLength();
}

uint8_t * FrontEnd::GetBytes(){
  return m_encoder->GetBytes();
}

