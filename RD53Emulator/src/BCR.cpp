#include "RD53Emulator/BCR.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

BCR::BCR(){
  m_symbol = 0x5959;
}

BCR::BCR(BCR * copy){
  m_symbol = copy->m_symbol;
}

BCR::~BCR(){}

BCR * BCR::Clone(){
  return new BCR(this);
}

string BCR::ToString(){
  ostringstream os;
  os << "BCR 0x" << hex << m_symbol << dec;
  return os.str();
}

uint32_t BCR::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  return 2; 
}

uint32_t BCR::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  return 2;
}

uint32_t BCR::GetType(){
  return Command::BCR;
}
