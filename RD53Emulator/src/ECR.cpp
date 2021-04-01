#include "RD53Emulator/ECR.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

ECR::ECR(){
  m_symbol = 0x5A5A;
}

ECR::ECR(ECR * copy){
  m_symbol = copy->m_symbol;
}

ECR::~ECR(){}

ECR * ECR::Clone(){
  return new ECR(this);
}

string ECR::ToString(){
  ostringstream os;
  os << "ECR 0x" << hex << m_symbol << dec;
  return os.str();
}

uint32_t ECR::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  return 2; 
}

uint32_t ECR::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  return 2;
}

uint32_t ECR::GetType(){
  return Command::ECR;
}
