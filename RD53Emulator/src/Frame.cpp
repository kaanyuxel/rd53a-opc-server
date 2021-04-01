#include "RD53Emulator/Frame.h"

using namespace RD53A;

Frame::Frame(){
  m_aheader=0;
}

Frame::~Frame(){}

uint8_t Frame::GetAuroraHeader(){
  return m_aheader;
}
