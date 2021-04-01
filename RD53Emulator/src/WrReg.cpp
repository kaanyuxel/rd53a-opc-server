#include "RD53Emulator/WrReg.h"
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace RD53A;

WrReg::WrReg(){
  m_symbol = 0x6666;
  m_chipid = 0;
  m_address = 0;
  m_mode = 0;
  m_value.resize(6,0);
}

WrReg::WrReg(WrReg * copy){
  m_symbol = copy->m_symbol;
  m_chipid = copy->m_chipid;
  m_address = copy->m_address;
  m_mode = copy->m_mode;
  m_value = copy->m_value;
}

WrReg::WrReg(uint32_t chipid, uint32_t address, uint32_t value){
  m_symbol = 0x6666;
  m_chipid = chipid;
  m_address = address;
  m_mode = 0;
  m_value.resize(6,0);
  m_value[0]=value;
}

WrReg::~WrReg(){}

WrReg * WrReg::Clone(){
  return new WrReg(this);
}

string WrReg::ToString(){
  ostringstream os;
  os << "WrReg 0x" << hex << m_symbol << dec
     << " ChipId:" << m_chipid
     << " (0x" << hex << m_chipid << dec << ")"
     << " Mode:" << m_mode
     << " (0x" << hex << m_mode << dec << ")"
     << " Address:" << m_address
     << " (0x" << hex << m_address << dec << ")"
     << " Value:";
  for(uint32_t i=0;i<(m_mode==0?1:6);i++){os << m_value[i] << ",";}
  os << " (0x" << hex;
  for(uint32_t i=0;i<(m_mode==0?1:6);i++){os << setw(4) << setfill('0') << m_value[i];}
  os << dec << ")";
  return os.str();
}

uint32_t WrReg::UnPack(uint8_t * bytes, uint32_t maxlen){
  if(maxlen<8) return 0; 
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  m_chipid   = m_symbol2data[bytes[2]]>>1;
  m_mode     = m_symbol2data[bytes[2]]&0x1;
  m_address  = m_symbol2data[bytes[3]]<<4;
  m_address |= m_symbol2data[bytes[4]]>>1;
  m_value[0] =(m_symbol2data[bytes[4]]&0x1)<<15;
  m_value[0]|= m_symbol2data[bytes[5]]<<10;
  m_value[0]|= m_symbol2data[bytes[6]]<<5;
  m_value[0]|= m_symbol2data[bytes[7]]<<0; 
  
  for(uint32_t i=1;i<5;i++){m_value[i] = 0;}
 if(m_mode==1 and maxlen>=24){

    m_value[1]|=((m_symbol2data[bytes[8] ]>>0)&0x1F)<<11;
    m_value[1]|=((m_symbol2data[bytes[9] ]>>0)&0x1F)<< 6;
    m_value[1]|=((m_symbol2data[bytes[10]]>>0)&0x1F)<< 1;
    m_value[1]|=((m_symbol2data[bytes[11]]>>4)&0x01)<< 0;

    m_value[2]|=((m_symbol2data[bytes[11]]>>0)&0x0F)<<12;
    m_value[2]|=((m_symbol2data[bytes[12]]>>0)&0x1F)<< 7;
    m_value[2]|=((m_symbol2data[bytes[13]]>>0)&0x1F)<< 2;
    m_value[2]|=((m_symbol2data[bytes[14]]>>3)&0x03)<< 0;

    m_value[3]|=((m_symbol2data[bytes[14]]>>0)&0x07)<<13;
    m_value[3]|=((m_symbol2data[bytes[15]]>>0)&0x1F)<< 8;
    m_value[3]|=((m_symbol2data[bytes[16]]>>0)&0x1F)<< 3;
    m_value[3]|=((m_symbol2data[bytes[17]]>>2)&0x07)<< 0;

    m_value[4]|=((m_symbol2data[bytes[17]]>>0)&0x03)<<14;
    m_value[4]|=((m_symbol2data[bytes[18]]>>0)&0x1F)<< 9;
    m_value[4]|=((m_symbol2data[bytes[19]]>>0)&0x1F)<< 4;
    m_value[4]|=((m_symbol2data[bytes[20]]>>1)&0x0F)<< 0;

    m_value[5]|=((m_symbol2data[bytes[20]]>>0)&0x01)<<15;
    m_value[5]|=((m_symbol2data[bytes[21]]>>0)&0x1F)<<10;
    m_value[5]|=((m_symbol2data[bytes[22]]>>0)&0x1F)<< 5;
    m_value[5]|=((m_symbol2data[bytes[23]]>>0)&0x1F)<< 0;
    return 24;
  }
  return 8; 
}

uint32_t WrReg::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  bytes[2]=m_data2symbol[(m_chipid<<1)|m_mode];
  bytes[3]=m_data2symbol[m_address>>4];
  bytes[4]=m_data2symbol[((m_address&0xF)<<1)|((m_value[0]>>15)&0x1)];
  bytes[5]=m_data2symbol[((m_value[0]>>10)&0x1F)];
  bytes[6]=m_data2symbol[((m_value[0]>> 5)&0x1F)];
  bytes[7]=m_data2symbol[((m_value[0]>> 0)&0x1F)];
  if(m_mode==1){
    bytes[ 8]=m_data2symbol[((m_value[1]>>11)&0x1F)];
    bytes[ 9]=m_data2symbol[((m_value[1]>> 6)&0x1F)];
    bytes[10]=m_data2symbol[((m_value[1]>> 1)&0x1F)];
    bytes[11]=m_data2symbol[((m_value[1]&0x01)<< 4)|((m_value[2]>>12)&0x0F)];

    bytes[12]=m_data2symbol[((m_value[2]>> 7)&0x1F)];
    bytes[13]=m_data2symbol[((m_value[2]>> 2)&0x1F)];
    bytes[14]=m_data2symbol[((m_value[2]&0x03)<< 3)|((m_value[3]>>13)&0x07)];

    bytes[15]=m_data2symbol[((m_value[3]>> 8)&0x1F)];
    bytes[16]=m_data2symbol[((m_value[3]>> 3)&0x1F)];
    bytes[17]=m_data2symbol[((m_value[3]&0x07)<< 2)|((m_value[4]>>14)&0x03)];

    bytes[18]=m_data2symbol[((m_value[4]>> 9)&0x1F)];
    bytes[19]=m_data2symbol[((m_value[4]>> 4)&0x1F)];
    bytes[20]=m_data2symbol[((m_value[4]&0x0F)<< 1)|((m_value[5]>>15)&0x01)];

    bytes[21]=m_data2symbol[((m_value[5]>>10)&0x1F)];
    bytes[22]=m_data2symbol[((m_value[5]>> 5)&0x1F)];
    bytes[23]=m_data2symbol[((m_value[5]>> 0)&0x1F)];
    return 24;
  }
  return 8;
}

uint32_t WrReg::GetType(){
  return Command::WRREG;
}

void WrReg::SetChipId(uint32_t chipid){
  m_chipid=chipid&0xF;
}

uint32_t WrReg::GetChipId(){
  return m_chipid;
}

void WrReg::SetAddress(uint32_t address){
  m_address=address&0x1FF;
}

uint32_t WrReg::GetAddress(){
  return m_address;
}

void WrReg::SetValue(uint32_t value, uint32_t index){
  m_value[index]=value&0xFFFF;
  if(index>0)m_mode=1;
}

uint32_t WrReg::GetValue(uint32_t index){
  return m_value[index];
}

void WrReg::SetMode(uint32_t mode){
  m_mode=mode&0x1;
}

uint32_t WrReg::GetMode(){
  return m_mode;
}




