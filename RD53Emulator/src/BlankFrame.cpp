#include "RD53Emulator/BlankFrame.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

BlankFrame::BlankFrame(){
  m_aheader = 0x2;
  m_afield = 0x1E;
}

BlankFrame::~BlankFrame(){}

string BlankFrame::ToString(){
  ostringstream os;
  os << "Blank " << hex
    /*<< "Header: 0x" << m_aheader << " "*/
     << "Code: 0x" << (uint32_t)m_afield << " "
     << dec;
  return os.str();
}

uint32_t BlankFrame::UnPack(uint8_t * bytes, uint32_t maxlen){
  if(bytes[0]!=0x1E || bytes[1]!=0x00){return 0;}
  m_afield=bytes[0];
  for(uint32_t i=2;i<8;i++){
    if(bytes[i]!=0){cout << "Blank::SetBytes Decoding error" << endl; return 0;}
  }
  return 8; 
}

uint32_t BlankFrame::Pack(uint8_t * bytes){
  bytes[0]= m_afield;
  for(uint32_t i=1;i<8;i++){
    bytes[i]=0;
  }
  return 8;
}

uint32_t BlankFrame::GetType(){
  return Frame::BLANK;
}
