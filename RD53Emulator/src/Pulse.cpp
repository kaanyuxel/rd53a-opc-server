#include "RD53Emulator/Pulse.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Pulse::Pulse(){
  m_symbol = 0x5C5C;
  m_chipid = 0;
  m_length = 0;
}

Pulse::Pulse(uint32_t chipid, uint32_t length){
  m_symbol = 0x5C5C;
  m_chipid = chipid;
  m_length = length;
}

Pulse::Pulse(Pulse * copy){
  m_symbol = copy->m_symbol;
  m_chipid = copy->m_chipid;
  m_length = copy->m_length;
}

Pulse::~Pulse(){}

Pulse * Pulse::Clone(){
  return new Pulse(this);
}

string Pulse::ToString(){
  ostringstream os;
  os << "Pulse 0x" << hex << m_symbol << dec
     << " ChipId:" << m_chipid
     << " (0x" << hex << m_chipid << dec << ")"
     << " Value:" << m_length
     << " (0x" << hex << m_length << dec << ")";
  return os.str();
}

uint32_t Pulse::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  if(maxlen<4) return 0;
  m_chipid =m_symbol2data[bytes[2]]>>1;
  m_length =m_symbol2data[bytes[3]]>>1;
  return 4; 
}

uint32_t Pulse::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  bytes[2]=m_data2symbol[m_chipid<<1];
  bytes[3]=m_data2symbol[m_length<<1];
  return 4;
}

uint32_t Pulse::GetType(){
  return Command::PULSE;
}

void Pulse::SetChipId(uint32_t chipid){
  m_chipid=chipid&0xF;
}

uint32_t Pulse::GetChipId(){
  return m_chipid;
}

void Pulse::SetLength(uint32_t length){
  m_length=length&0xF;
}

uint32_t Pulse::GetLength(){
  return m_length;
}


