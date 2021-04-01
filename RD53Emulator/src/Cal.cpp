#include "RD53Emulator/Cal.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Cal::Cal(){
  m_symbol = 0x6363;
  m_chipid = 0;
  m_edge_mode = 0;
  m_edge_delay = 0;
  m_edge_width = 0;
  m_aux_mode = 0;
  m_aux_delay = 0;
}

Cal::Cal(Cal * copy){
  m_symbol = copy->m_symbol;
  m_chipid = copy->m_chipid;
  m_edge_mode = copy->m_edge_mode;
  m_edge_delay = copy->m_edge_delay;
  m_edge_width = copy->m_edge_width;
  m_aux_mode = copy->m_aux_mode;
  m_aux_delay = copy->m_aux_delay;
}

Cal::Cal(uint32_t chipid, uint32_t edge_mode, uint32_t edge_delay, uint32_t edge_width, uint32_t aux_mode, uint32_t aux_delay){
  m_symbol = 0x6363;
  m_chipid = chipid;
  m_edge_mode = edge_mode;
  m_edge_delay = edge_delay;
  m_edge_width = edge_width;
  m_aux_mode = aux_mode;
  m_aux_delay = aux_delay;
}

Cal::~Cal(){}

Cal * Cal::Clone(){
  return new Cal(this);
}

string Cal::ToString(){
  ostringstream os;
  os << "Cal 0x" << hex << m_symbol << dec
     << " ChipId:" << m_chipid
     << " (0x" << hex << m_chipid << dec << ")"
     << " edge_mode:" << m_edge_mode
     << " (0x" << hex << m_edge_mode << dec << ")"
     << " edge_delay:" << m_edge_delay
     << " (0x" << hex << m_edge_delay << dec << ")"
     << " edge_width:" << m_edge_width
     << " (0x" << hex << m_edge_width << dec << ")"
     << " aux_mode:" << m_aux_mode
     << " (0x" << hex << m_aux_mode << dec << ")"
     << " aux_delay:" << m_aux_delay
     << " (0x" << hex << m_aux_delay << dec << ")";
  return os.str();
}

uint32_t Cal::UnPack(uint8_t * bytes, uint32_t maxlen){
  if (bytes[0]!=((m_symbol>>0)&0xFF) ||
      bytes[1]!=((m_symbol>>8)&0xFF)) return 0;
  if(maxlen<6) return 0;
  m_chipid     =(m_symbol2data[bytes[2]]>>1);
  m_edge_mode  =(m_symbol2data[bytes[2]]>>0)&0x1;
  m_edge_delay =(m_symbol2data[bytes[3]]>>2)&0x7;
  m_edge_width =(m_symbol2data[bytes[3]]&0x3)<<4;
  m_edge_width|=(m_symbol2data[bytes[4]]>>1)&0xF;
  m_aux_mode   =(m_symbol2data[bytes[4]]>>0)&0x1;
  m_aux_delay  =(m_symbol2data[bytes[5]]>>0)&0x1F;
  return 6; 
}

uint32_t Cal::Pack(uint8_t * bytes){
  bytes[0]=((m_symbol>>0)&0xFF);
  bytes[1]=((m_symbol>>8)&0xFF);
  bytes[2]=m_data2symbol[((m_chipid&0xF)<<1)    |(m_edge_mode&0x1)];
  bytes[3]=m_data2symbol[((m_edge_delay&0x7)<<2)|(m_edge_width>>4)];
  bytes[4]=m_data2symbol[((m_edge_width&0xF)<<1)|(m_edge_mode&0x1)];
  bytes[5]=m_data2symbol[((m_edge_delay&0x1F)<<0)];
  return 6;
}

uint32_t Cal::GetType(){
  return Command::CAL;
}

void Cal::SetChipId(uint32_t chipid){
  m_chipid=chipid&0xF;
}

uint32_t Cal::GetChipId(){
  return m_chipid;
}

void Cal::SetEdgeMode(uint32_t edge_mode){
  m_edge_mode=edge_mode&0x1;
}

uint32_t Cal::GetEdgeMode(){
  return m_edge_mode;
}

void Cal::SetEdgeDelay(uint32_t edge_delay){
  m_edge_delay=edge_delay&0x7;
}

uint32_t Cal::GetEdgeDelay(){
  return m_edge_delay;
}

void Cal::SetEdgeWidth(uint32_t edge_width){
  m_edge_width=edge_width&0x3F;
}

uint32_t Cal::GetEdgeWidth(){
  return m_edge_width;
}

void Cal::SetAuxMode(uint32_t aux_mode){
  m_aux_mode=aux_mode&0x1;
}

uint32_t Cal::GetAuxMode(){
  return m_aux_mode;
}

void Cal::SetAuxDelay(uint32_t aux_delay){
  m_aux_delay=aux_delay&0x1F;
}

uint32_t Cal::GetAuxDelay(){
  return m_aux_delay;
}
