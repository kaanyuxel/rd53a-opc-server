#include "RD53Emulator/RegisterFrame.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

const char * const RegisterFrame::m_status2text[] = {
  "Ready", 
  "There has been an error since the last register frame",
  "There has been a warning since the last register frame",
  "There has been an error and a warning since the last register frame",
  "Ready",
  "Trigger queue is full (Busy)",
  "No input channel lock",
  "Spare","Spare","Spare","Spare","Spare","Spare","Spare","Spare","Spare"};

std::map<uint32_t,const char*> RegisterFrame::m_acode2text = {
  {0xB4,"Both register fields are of auto-read"},
  {0x55,"First frame is auto-read, second is from a read register command"},
  {0x99,"First is from a read register command, second frame is auto-read"},
  {0xD2,"Both register fields are from read register commands"},
  {0xCC,"Error. Fields are meaningless."}
};

RegisterFrame::RegisterFrame(){
  m_aheader = 0x2;
  m_acode = 0;
  m_status = 0;
  m_err=0;
  m_addr[0]=0;
  m_addr[1]=0;
  m_vals[0]=0;
  m_vals[1]=0;
  m_auto[0]=0;
  m_auto[1]=0;
}

RegisterFrame::RegisterFrame(RegisterFrame *copy){
  m_aheader = copy->m_aheader;
  m_acode = copy->m_acode;
  m_status = copy->m_status;
  m_err = copy->m_err;
  m_addr[0] = copy->m_addr[0];
  m_addr[1] = copy->m_addr[1];
  m_vals[0] = copy->m_vals[0];
  m_vals[1] = copy->m_vals[1];
  m_auto[0] = copy->m_auto[0];
  m_auto[1] = copy->m_auto[1];
}

RegisterFrame::RegisterFrame(uint32_t code, uint32_t status, uint32_t address1, uint32_t value1, uint32_t address2, uint32_t value2){
  m_aheader = 0x2;
  m_acode = code;
  m_status = status;
  m_addr[0] = address1;
  m_addr[1] = address2;
  m_vals[0] = value1;
  m_vals[1] = value2;
  UnPack();
}

RegisterFrame::~RegisterFrame(){}

string RegisterFrame::ToString(){
  ostringstream os;
  os << "RegisterFrame " << hex
    /* << "Header: 0x" << m_aheader << " " */
     << "Code: 0x" << (uint32_t)m_acode << " "
     << "Status: 0x" << (uint32_t)m_status << " "
     << "(" << m_status2text[m_status] << ") "
     << "Addr1: 0x" << setw(3) << setfill('0') << m_addr[0] << " "
     << "Val1: 0x" << setw(4) << setfill('0') << m_vals[0] << " "
     << (m_auto[0]?"(auto) ":"")
     << "Addr2: 0x" << setw(3) << setfill('0') << m_addr[1] << " "
     << "Val2: 0x" << setw(4) << setfill('0') << m_vals[1] << " "
     << (m_auto[1]?"(auto) ":"")
     << (m_err?"(error) ":"")
     << dec;
  return os.str();
}

void RegisterFrame::Pack(){
  if     (m_auto[0]==1 && m_auto[1]==1 && m_err==0){m_acode=0xB4;}
  else if(m_auto[0]==1 && m_auto[1]==0 && m_err==0){m_acode=0x55;}
  else if(m_auto[0]==0 && m_auto[1]==1 && m_err==0){m_acode=0x99;}
  else if(m_auto[0]==0 && m_auto[1]==0 && m_err==0){m_acode=0xD2;}
  else if(                                m_err==1){m_acode=0xCC;}
}

bool RegisterFrame::UnPack(){
  switch(m_acode){
  case 0xB4: m_auto[0]=1; m_auto[1]=1; m_err=0; break;
  case 0x55: m_auto[0]=1; m_auto[1]=0; m_err=0; break;
  case 0x99: m_auto[0]=0; m_auto[1]=1; m_err=0; break;
  case 0xD2: m_auto[0]=0; m_auto[1]=0; m_err=0; break;
  case 0xCC: m_auto[0]=0; m_auto[1]=0; m_err=1; break;
  default:   m_auto[0]=0; m_auto[1]=0; m_err=1; return false;
  }
  return true;
}

uint32_t RegisterFrame::UnPack(uint8_t * bytes, uint32_t maxlen){
  m_acode=bytes[0];
  if(!UnPack()){return 0;}
  for(uint32_t i=0;i<2;i++){m_addr[i]=0;m_vals[i]=0;}
  m_status  = (bytes[1]>>4)&0xFF;
  m_addr[0] =((bytes[1]&0x0F)<< 6) | ((bytes[2]&0xFC)>> 2);
  m_vals[0] =((bytes[2]&0x03)<<14) | ((bytes[3]&0xFF)<< 6) | ((bytes[4]&0xFC)>> 2);
  m_addr[1] =((bytes[4]&0x03)<< 8) | ((bytes[5]&0xFF)>> 0);
  m_vals[1] =((bytes[6]&0xFF)<< 8) | ((bytes[7]&0xFF)>> 0);
  return 8;
}

uint32_t RegisterFrame::Pack(uint8_t * bytes){
  for(uint32_t i=0;i<8;i++){bytes[i]=0;}
  Pack();
  bytes[0] =((m_acode  << 0)&0xFF);
  bytes[1] =((m_status << 4)&0xF0) | ((m_addr[0]>> 6)&0x0F);
  bytes[2] =((m_addr[0]<< 2)&0xFC) | ((m_vals[0]>>14)&0x03);
  bytes[3] =((m_vals[0]>> 6)&0xFF);
  bytes[4] =((m_vals[0]<< 2)&0xFC) | ((m_addr[1]>> 8)&0x03);
  bytes[5] =((m_addr[1]<< 0)&0xFF);
  bytes[6] =((m_vals[1]>> 8)&0xFF);
  bytes[7] =((m_vals[1]>> 0)&0xFF);
  return 8;
}

uint32_t RegisterFrame::GetType(){
  return Frame::REGISTER;
}

void RegisterFrame::SetAuroraCode(uint32_t code){
  m_acode=code&0xFF;
  UnPack();
}

uint32_t RegisterFrame::GetAuroraCode(){
  Pack();
  return m_acode;
}

std::string RegisterFrame::GetAuroraCodeString(){
  Pack();
  if(m_acode2text.count(m_acode)==0)return "";
  return m_acode2text[m_acode];
}

void RegisterFrame::SetRegister(uint32_t pos, uint32_t addr, uint32_t val, bool autoread){
  if(pos>1) return;
  m_addr[pos]=addr;
  m_vals[pos]=val;
  m_auto[pos]=autoread;
}

void RegisterFrame::SetAddress(uint32_t pos, uint32_t addr){
  if(pos>1) return;
  m_addr[pos]=addr;
}

void RegisterFrame::SetValue(uint32_t pos, uint32_t val){
  if(pos>1) return;
  m_vals[pos]=val;
}

uint32_t RegisterFrame::GetAddress(uint32_t pos){
  if(pos>1) return 0;
  return m_addr[pos];
}

uint32_t RegisterFrame::GetValue(uint32_t pos){
  if(pos>1) return 0;
  return m_vals[pos];
}

void RegisterFrame::SetError(bool error){
  m_err=error;
}

bool RegisterFrame::GetError(){
  return m_err;
}

bool RegisterFrame::GetAuto(uint32_t pos){
  if(pos>1)return 0;
  return m_auto[pos];
}

void RegisterFrame::SetAuto(uint32_t pos, bool enable){
  if(pos>1)return;
  m_auto[pos]=enable;
}

void RegisterFrame::SetStatus(uint32_t status){
  if(status>0xF){return;}
  m_status=status;
}

uint32_t RegisterFrame::GetStatus(){
  return m_status;
}

std::string RegisterFrame::GetStatusString(){
  return string(m_status2text[m_status]);
}
