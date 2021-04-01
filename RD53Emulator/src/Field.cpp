#include "RD53Emulator/Field.h"

using namespace std;
using namespace RD53A;

vector<uint32_t> Field::m_masks = {
  0x0000,
  0x0001,0x0003,0x0007,0x000F,
  0x001F,0x003F,0x007F,0x00FF,
  0x01FF,0x03FF,0x07FF,0x0FFF,
  0x1FFF,0x3FFF,0x7FFF,0xFFFF,
};

Field::Field(Register * data, uint32_t start, uint32_t len, uint32_t defval, bool reversed){
  m_data=data;
  m_start=start;
  m_len=len;
  m_defval=defval;
  m_mask=m_masks[len];
  m_smask=m_mask<<m_start;
  m_reversed=reversed;
  SetValue(m_defval);
}

Field::~Field(){}

uint32_t Field::Reverse(uint32_t value, uint32_t sz){
  uint32_t ret=0;
  for(uint32_t i=0;i<sz;i++){
    ret |= ((value>>i)&0x1)<<(sz-i-1);
  }
  //cout << "sz:" << sz << " value:" << value << " reversed:" << ret << endl;
  return ret;
}

void Field::SetValue(uint32_t value){
  if(m_reversed){value=Reverse(value,m_len);}
  uint32_t val=m_data->GetValue();
  val&=~(m_smask);
  val|=((value&m_mask)<<m_start);
  m_data->SetValue(val);
}

uint32_t Field::GetValue(){
  uint32_t value = (m_data->GetValue()>>m_start)&m_mask;
  if(m_reversed){value=Reverse(value,m_len);}
  return value;
}
