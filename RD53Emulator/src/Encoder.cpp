#include "RD53Emulator/Encoder.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Encoder::Encoder(){
  m_bytes.reserve(1000000);
  m_bytes.resize(1000000,0);
  m_length = 0;
  m_ecr = new ECR;
  m_bcr = new BCR;
  m_cal = new Cal;
  m_noop = new Noop;
  m_sync = new Sync;
  m_pulse = new Pulse;
  m_wrreg = new WrReg;
  m_rdreg = new RdReg;
  m_trig = new Trigger;
}

Encoder::~Encoder(){
  m_bytes.clear();
  delete m_ecr;
  delete m_bcr;
  delete m_cal;
  delete m_noop;
  delete m_sync;
  delete m_pulse;
  delete m_wrreg;
  delete m_rdreg;
  delete m_trig;
  ClearCommands();
}

void Encoder::AddBytes(uint8_t *bytes, uint32_t pos, uint32_t len){
  for(uint32_t i=pos;i<pos+len;i++){
    m_bytes[m_length]=bytes[i];
    m_length++;
  }
}

void Encoder::SetBytes(uint8_t *bytes, uint32_t len){
  m_length=0;
  for(uint32_t i=0;i<len;i++){
    m_bytes[m_length]=bytes[i];
    m_length++;
  }
}

void Encoder::AddCommand(Command *cmd){
  m_cmds.push_back(cmd);
}
  
void Encoder::ClearBytes(){
  m_length=0;
}
  
void Encoder::ClearCommands(){
  while(!m_cmds.empty()){
    Command* cmd = m_cmds.back();
    delete cmd;
    m_cmds.pop_back();
  }
}

void Encoder::Clear(){
  ClearBytes();
  ClearCommands();
}
  
string Encoder::GetByteString(){
  ostringstream os;
  os << hex; 
  for(uint32_t pos=0; pos<m_length; pos++){
    os << setw(2) << setfill('0') << (uint32_t) m_bytes[pos] << " ";
  }
  return os.str();
}

uint8_t * Encoder::GetBytes(){
  return m_bytes.data();
}
  
uint32_t Encoder::GetLength(){
  return m_length;
}
  
vector<Command*> & Encoder::GetCommands(){
  return m_cmds;
}

void Encoder::Encode(){
  uint32_t pos=0;
  for(uint32_t i=0;i<m_cmds.size();i++){
    pos+=m_cmds[i]->Pack(&m_bytes[pos]);
  }
  m_length=pos;
}

void Encoder::Decode(){
  ClearCommands();
  uint32_t pos=0;
  uint32_t nb=0;
  uint32_t tnb=m_length;
  while(pos!=tnb){
    Command * cmd=0;
    if     ((nb=m_cal->  UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_cal; m_cal=new Cal();}
    else if((nb=m_ecr->  UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_ecr; m_ecr=new ECR();}
    else if((nb=m_bcr->  UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_bcr; m_bcr=new BCR();}
    else if((nb=m_pulse->UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_pulse; m_pulse=new Pulse();}
    else if((nb=m_rdreg->UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_rdreg; m_rdreg=new RdReg();}
    else if((nb=m_wrreg->UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_wrreg; m_wrreg=new WrReg();}
    else if((nb=m_noop-> UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_noop; m_noop=new Noop();}
    else if((nb=m_sync-> UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_sync; m_sync=new Sync();}
    else if((nb=m_trig-> UnPack(&m_bytes[pos],tnb-pos))>0){cmd=m_trig; m_trig=new Trigger();}
    else{
       cout << __PRETTY_FUNCTION__ << "Cannot decode byte sequence: "
            << "0x" << hex << setw(2) << setfill('0') << (uint32_t) m_bytes[pos] << dec
            << " at index: " << pos
            << " ...skipping" << endl;
       pos++;
       continue;
    }
    if(!cmd){pos++; continue;}
    m_cmds.push_back(cmd);
    pos+=nb;
    //Check if there is bytes left
  }
}
