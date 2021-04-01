#include "RD53Emulator/Noop.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Noop::Noop(){
  m_symbol = 0x6969;
}

Noop::Noop(Noop * copy){
  m_symbol = copy->m_symbol;
}

Noop::~Noop(){}

Noop * Noop::Clone(){
  return new Noop(this);
}

string Noop::ToString(){
  ostringstream os;
  os << "Noop 0x" << hex << m_symbol << dec;
  return os.str();
}

uint32_t Noop::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  return 2; 
}

uint32_t Noop::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  return 2;
}

uint32_t Noop::GetType(){
  return Command::NOOP;
}

