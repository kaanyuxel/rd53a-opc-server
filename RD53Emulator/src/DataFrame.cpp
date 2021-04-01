#include "RD53Emulator/DataFrame.h"
#include <sstream>
#include <iomanip>
#include <bitset>
#include <iostream>

using namespace std;
using namespace RD53A;

DataFrame::DataFrame(){
  m_aheader = 0x2;
  m_format = UNKNOWN;

  for(uint32_t i=0;i<2;i++){
    m_ccol[i] = 0;
    m_crow[i] = 0;
    m_creg[i] = 0;
    m_tot1[i] = 0;
    m_tot2[i] = 0;
    m_tot3[i] = 0;
    m_tot4[i] = 0;
    m_TID[i] = 0;
    m_TTag[i] = 0;
    m_BCID[i] = 0;
  }
}

DataFrame::~DataFrame(){}

string DataFrame::ToString(){
  ostringstream os;
  os << "DataFrame " /*<< "Header 0x" << hex << m_aheader << dec */;

  //Position 1
  switch(m_format){
  case SYN_HIT:
  case SYN_HDR: 
    os << "Syn";
    break;
  case HDR_HIT:
  case HDR_HDR:
    os << "Hdr" 
       << " Trig ID 0x" << hex << m_TID[0] << dec
       << " Trig Tag 0x" << hex <<m_TTag[0] << dec
       << " BCID 0x" << hex << m_BCID[0] << dec
       << " |";
    break;
  case HIT_HIT:
  case HIT_HDR:
    os << " Hit Col 0x" << hex << m_ccol[0] << dec
       << " Row 0x" << hex << m_crow[0] << dec
       << " Reg 0x" << hex << m_creg[0] << dec
       << " TOT1 0x" << hex << m_tot1[0] << dec
       << " TOT2 0x" << hex << m_tot2[0] << dec
       << " TOT3 0x" << hex << m_tot3[0] << dec
       << " TOT4 0x" << hex << m_tot4[0] << dec;
    break;
  case UNKNOWN:
    os << "Unknown";
    break;
  }

  //Position 2
  switch(m_format){
  case SYN_HIT:
  case HDR_HIT:
  case HIT_HIT:
    os << " Hit Col 0x" << hex << m_ccol[1] << dec
       << " Row 0x" << hex << m_crow[1] << dec
       << " Reg 0x" << hex << m_creg[1] << dec
       << " TOT1 0x" << hex << m_tot1[1] << dec
       << " TOT2 0x" << hex << m_tot2[1] << dec
       << " TOT3 0x" << hex << m_tot3[1] << dec
       << " TOT4 0x" << hex << m_tot4[1] << dec;
    break;
  case SYN_HDR: 
  case HDR_HDR:
  case HIT_HDR:
    os << " Hdr" 
       << " Trigger ID 0x" << hex << m_TID[1] << dec
       << " Trigger Tag 0x" << hex <<m_TTag[1] << dec
       << " BCID 0x" << hex << m_BCID[1] << dec;
    break;
  case UNKNOWN:
    os << " Unknown";
    break;
  }
  
  return os.str();
}

uint32_t DataFrame::Pack(uint8_t * bytes){

  for(uint32_t i=0;i<8;i++){bytes[i]=0;}

  //position 1
  switch(m_format){
  case SYN_HIT:
  case SYN_HDR:
    bytes[0]=0x1E;
    bytes[1]=0x04;
    bytes[2]=0;
    bytes[3]=0;
    break;
  case HDR_HDR:
  case HDR_HIT:
    bytes[0]  =  0x2;
    bytes[0] |= (m_TID[0] >>4)&0x01;
    bytes[1]  = (m_TID[0] <<4)&0xF0;
    bytes[1] |= (m_TTag[0]>>1)&0x0F;
    bytes[2]  = (m_TTag[0]<<7)&0x80;
    bytes[2] |= (m_BCID[0]>>8)&0x7F;
    bytes[3] |= (m_BCID[0]>>0)&0xFF;
    break;
  case HIT_HDR:
  case HIT_HIT:
    bytes[0]  = (m_ccol[0]<<2)&0xFC;
    bytes[0] |= (m_crow[0]>>4)&0x03;
    bytes[1]  = (m_crow[0]<<4)&0xF0;
    bytes[1] |= (m_creg[0]>>0)&0x0F;
    bytes[2]  = (m_tot1[0]<<4)&0xF0;
    bytes[2] |= (m_tot2[0]>>0)&0x0F;
    bytes[3]  = (m_tot3[0]<<4)&0xF0;
    bytes[3] |= (m_tot4[0]>>0)&0x0F;
    break;
  case UNKNOWN:
    bytes[0]=0;
    bytes[1]=0;
    bytes[2]=0;
    bytes[3]=0;
  }
  //position 2
  switch(m_format){
  case SYN_HIT:
  case HIT_HIT:
  case HDR_HIT:
    bytes[4]  = (m_ccol[1]<<2)&0xFC;
    bytes[4] |= (m_crow[1]>>4)&0x03;
    bytes[5]  = (m_crow[1]<<4)&0xF0;
    bytes[5] |= (m_creg[1]>>0)&0x0F;
    bytes[6]  = (m_tot1[1]<<4)&0xF0;
    bytes[6] |= (m_tot2[1]>>0)&0x0F;
    bytes[7]  = (m_tot3[1]<<4)&0xF0;
    bytes[7] |= (m_tot4[1]>>0)&0x0F;
    break;
  case SYN_HDR:
  case HIT_HDR:
  case HDR_HDR:
    bytes[4]  =  0x2;
    bytes[4] |= (m_TID[1] >>4)&0x01;
    bytes[5]  = (m_TID[1] <<4)&0xF0;
    bytes[5] |= (m_TTag[1]>>1)&0x0F;
    bytes[6]  = (m_TTag[1]<<7)&0x80;
    bytes[6] |= (m_BCID[1]>>8)&0x7F;
    bytes[7] |= (m_BCID[1]>>0)&0xFF;
    break;
  case UNKNOWN:
    bytes[4]=0;
    bytes[5]=0;
    bytes[6]=0;
    bytes[7]=0;
    break;
  }
  
  return 8;
}

uint32_t DataFrame::UnPack(uint8_t * bytes, uint32_t maxlen){

  m_format=UNKNOWN;
  //@todo Check this logic again
  if     (bytes[0]==0x1E && bytes[4]==0x2){m_format=SYN_HDR;}
  else if(bytes[0]==0x1E && bytes[4]!=0x2){m_format=SYN_HIT;}
  else if(bytes[0]==0x02 && bytes[4]==0x2){m_format=HDR_HDR;}
  else if(bytes[0]==0x02 && bytes[4]!=0x2){m_format=HDR_HIT;}
  else if(bytes[0]!=0x02 && bytes[4]==0x2){m_format=HIT_HDR;}
  else if(bytes[0]!=0x02 && bytes[4]!=0x2){m_format=HIT_HIT;}
  else                                    {return 0;}

  //position 1
  if(m_format==HDR_HDR || m_format==HDR_HIT){
    m_TID[0]   = (bytes[0]&0x01)<<4;
    m_TID[0]  |= (bytes[1]&0xF0)>>4;
    m_TTag[0]  = (bytes[1]&0x0F)<<1;
    m_TTag[0] |= (bytes[2]&0x80)>>7;
    m_BCID[0]  = (bytes[2]&0x7F)<<8;
    m_BCID[0] |= (bytes[3]&0xFF)<<0;
  }else if(m_format==HIT_HDR){
    m_ccol[0]  = (bytes[0]&0xFC)>>2;
    m_crow[0]  = (bytes[0]&0x03)<<4;
    m_crow[0] |= (bytes[1]&0xF0)>>4;
    m_creg[0]  = (bytes[1]&0x0F)<<0;
    m_tot1[0]  = (bytes[2]&0xF0)>>4;
    m_tot2[0]  = (bytes[2]&0x0F)<<0;
    m_tot3[0]  = (bytes[3]&0xF0)>>4;
    m_tot4[0]  = (bytes[3]&0x0F)<<0;
  }
  else if(m_format==SYN_HIT){
    // EJS: anything to do here?
    // it's a sync signal, so I guess there is no info to readout
  }

  //position 2
  if(m_format==SYN_HDR || m_format==HDR_HDR){
    m_TID[1]   = (bytes[4]&0x01)<<4;
    m_TID[1]  |= (bytes[5]&0xF0)>>4;
    m_TTag[1]  = (bytes[5]&0x0F)<<1;
    m_TTag[1] |= (bytes[6]&0x80)>>7;
    m_BCID[1]  = (bytes[6]&0x7F)<<8;
    m_BCID[1] |= (bytes[7]&0xFF)<<0;
  }else if(m_format==HDR_HIT || m_format==SYN_HIT){
    m_ccol[1]  = (bytes[4]&0xFC)>>2;
    m_crow[1]  = (bytes[4]&0x03)<<4;
    m_crow[1] |= (bytes[5]&0xF0)>>4;
    m_creg[1]  = (bytes[5]&0x0F)<<0;
    m_tot1[1]  = (bytes[6]&0xF0)>>4;
    m_tot2[1]  = (bytes[6]&0x0F)<<0;
    m_tot3[1]  = (bytes[7]&0xF0)>>4;
    m_tot4[1]  = (bytes[7]&0x0F)<<0;
  }
  
  return 8; 
}

uint32_t DataFrame::GetType(){
  return Frame::DATA;
}

void DataFrame::SetFormat(uint32_t format){
  m_format=format;
}

uint32_t DataFrame::GetFormat(){
  return m_format;
}

void DataFrame::SetHeader(uint32_t pos, uint32_t triggerID, uint32_t triggerTag, uint32_t BCID){
  m_TID[pos] = triggerID&0x1F;
  m_TTag[pos] = triggerTag&0x1F;
  m_BCID[pos] = BCID&0x7FFF;
}

void DataFrame::SetHit(uint32_t pos, uint32_t core_col, uint32_t core_row, uint32_t core_region, uint32_t tot1, uint32_t tot2, uint32_t tot3, uint32_t tot4){
  if (pos>1) return;
  m_ccol[pos] = core_col&0x3F;
  m_crow[pos] = core_row&0x3F;
  m_creg[pos] = core_region&0x0F;
  m_tot1[pos] = tot1&0x0F;
  m_tot2[pos] = tot2&0x0F;
  m_tot3[pos] = tot3&0x0F;
  m_tot4[pos] = tot4&0x0F;
}

void DataFrame::SetHit(uint32_t pos, uint32_t quad_col, uint32_t row, uint32_t * tot){
  if (pos>1) return;
  m_ccol[pos] = (quad_col>>1)&0x3F;
  m_crow[pos] = (row>>3)&0x3F;
  m_creg[pos] = ((row<<1)|(quad_col&0x1))&0x0F;
  m_tot1[pos] = tot[0]&0x0F;
  m_tot2[pos] = tot[1]&0x0F;
  m_tot3[pos] = tot[2]&0x0F;
  m_tot4[pos] = tot[3]&0x0F;
}

void DataFrame::SetTID(uint32_t pos, uint32_t triggerID){
  m_TID[pos] = triggerID&0x1F;
}

void DataFrame::SetTTag(uint32_t pos, uint32_t triggerTag){
  m_TTag[pos] = triggerTag&0x1F;
}

void DataFrame::SetBCID(uint32_t pos, uint32_t BCID){
  m_BCID[pos] = BCID&0x7FFF;
}

void DataFrame::SetCoreCol(uint32_t pos, uint32_t ccol) {
  m_ccol[pos] = ccol&0x3F;
}

void DataFrame::SetCoreRow(uint32_t pos, uint32_t crow) {
  m_crow[pos] = crow&0x1FF;
}

void DataFrame::SetCoreReg(uint32_t pos, uint32_t creg) {
  m_creg[pos] = creg&0x01;
}

void DataFrame::SetTOT1(uint32_t pos, uint32_t tot) {
  m_tot1[pos] = tot&0x0F;
}

void DataFrame::SetTOT2(uint32_t pos, uint32_t tot) {
  m_tot2[pos] = tot&0x0F;
}

void DataFrame::SetTOT3(uint32_t pos, uint32_t tot) {
  m_tot3[pos] = tot&0x0F;
}

void DataFrame::SetTOT4(uint32_t pos, uint32_t tot) {
  m_tot4[pos] = tot&0x0F;
}

void DataFrame::SetTOT(uint32_t pos, uint32_t idx, uint32_t tot) {
  switch(idx){
  case 0: m_tot1[pos]=tot;
  case 1: m_tot2[pos]=tot;
  case 2: m_tot3[pos]=tot;
  case 3: m_tot4[pos]=tot;
  }
}

uint32_t DataFrame::GetTID(uint32_t pos) {
  return m_TID[pos];
}

uint32_t DataFrame::GetTTag(uint32_t pos) {
  return m_TTag[pos];
}

uint32_t DataFrame::GetBCID(uint32_t pos) {
  return m_BCID[pos];
}

uint32_t DataFrame::GetCoreCol(uint32_t pos) {
  return m_ccol[pos];
}

uint32_t DataFrame::GetCoreRow(uint32_t pos) {
  return m_crow[pos];
}

uint32_t DataFrame::GetCoreReg(uint32_t pos) {
  return m_creg[pos];
}

uint32_t DataFrame::GetTOT1(uint32_t pos) {
  return m_tot1[pos];
}

uint32_t DataFrame::GetTOT2(uint32_t pos) {
  return m_tot2[pos];
}

uint32_t DataFrame::GetTOT3(uint32_t pos) {
  return m_tot3[pos];
}

uint32_t DataFrame::GetTOT4(uint32_t pos) {
  return m_tot4[pos];
}

uint32_t DataFrame::GetTOT(uint32_t pos, uint32_t idx) {
  switch(idx){
  case 0: return m_tot1[pos];
  case 1: return m_tot2[pos];
  case 2: return m_tot3[pos];
  case 3: return m_tot4[pos];
  default: return 0;
  }
}

uint32_t DataFrame::GetCol(uint32_t pos) {
  return m_ccol[pos]*8+(m_creg[pos]>>3)*4;
}

uint32_t DataFrame::GetRow(uint32_t pos) {
  return m_crow[pos]*8+(m_creg[pos]&0x7);
}
