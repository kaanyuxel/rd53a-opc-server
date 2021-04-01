#include "RD53Emulator/RdReg.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

RdReg::RdReg(){
  m_symbol = 0x6565;
  m_chipid = 0;
  m_address = 0;
}

RdReg::RdReg(RdReg * copy){
  m_symbol = copy->m_symbol;
  m_chipid = copy->m_chipid;
  m_address = copy->m_address;
}

RdReg::RdReg(uint32_t chipid, uint32_t address){
  m_symbol = 0x6565;
  m_chipid=chipid&0xF;
  m_address=address&0x1FF;
}

RdReg::~RdReg(){}

RdReg * RdReg::Clone(){
  return new RdReg(this);
}

string RdReg::ToString(){
  ostringstream os;
  os << "RdReg 0x" << hex << m_symbol << dec
     << " ChipId:" << m_chipid
     << " (0x" << hex << m_chipid << dec << ")"
     << " Address:" << m_address
     << " (0x" << hex << m_address << dec << ")";
  return os.str();
}

uint32_t RdReg::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  m_chipid   =(m_symbol2data[bytes[2]]>>1);
  m_address  =(m_symbol2data[bytes[3]]<<4);
  m_address |=(m_symbol2data[bytes[4]]>>1);
  return 6;
}

uint32_t RdReg::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  bytes[2]=m_data2symbol[m_chipid<<1];
  bytes[3]=m_data2symbol[m_address>>4];
  bytes[4]=m_data2symbol[(m_address&0xF)<<1];
  bytes[5]=m_data2symbol[0];
  return 6;
}

uint32_t RdReg::GetType(){
  return Command::RDREG;
}

void RdReg::SetChipId(uint32_t chipid){
  m_chipid=chipid&0xF;
}

uint32_t RdReg::GetChipId(){
  return m_chipid;
}

void RdReg::SetAddress(uint32_t address){
  m_address=address&0x1FF;
}

uint32_t RdReg::GetAddress(){
  return m_address;
}



