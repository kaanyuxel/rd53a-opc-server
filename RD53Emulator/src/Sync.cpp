#include "RD53Emulator/Sync.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Sync::Sync(){
  m_symbol = 0x417E;
}

Sync::Sync(Sync * copy){
  m_symbol = copy->m_symbol;
}

Sync::~Sync(){}

Sync * Sync::Clone(){
  return new Sync(this);
}

string Sync::ToString(){
  ostringstream os;
  os << "Sync 0x" << hex << m_symbol << dec;
  return os.str();
}

uint32_t Sync::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[1]!=((m_symbol>>0)&0xFF) ||
      bytes[0]!=((m_symbol>>8)&0xFF)) return 0;
  return 2;
}

uint32_t Sync::Pack(uint8_t * bytes){
  bytes[1]=((m_symbol>>0)&0xFF);
  bytes[0]=((m_symbol>>8)&0xFF);
  return 2;
}

uint32_t Sync::GetType(){
  return Command::SYNC;
}

