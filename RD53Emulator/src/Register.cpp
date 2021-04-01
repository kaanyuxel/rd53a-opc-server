#include "RD53Emulator/Register.h"

using namespace std;
using namespace RD53A;

Register::Register(){
  m_value=0;
  m_updated=false;
}

Register::~Register(){}

void Register::SetValue(uint16_t value){
  m_updated=true;
  m_value=value;
}

uint16_t Register::GetValue(){
  return m_value;
}

void Register::Update(bool enable){
  m_updated=enable;
}

bool Register::IsUpdated(){
  return m_updated;
}
