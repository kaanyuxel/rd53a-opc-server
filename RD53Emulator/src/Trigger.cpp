#include "RD53Emulator/Trigger.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Trigger::Trigger(){
  m_pattern=Trigger_08;
  m_tag=0;
  m_bcs={1,0,0,0};
}

Trigger::Trigger(Trigger * copy){
  m_pattern=copy->m_pattern;
  m_tag=copy->m_tag;
  m_bcs=copy->m_bcs;
}

Trigger::Trigger(uint32_t pattern, uint32_t tag){
  m_pattern=pattern;
  m_tag=tag;
}

Trigger::Trigger(bool bc1, bool bc2, bool bc3, bool bc4, uint32_t tag){
  m_pattern=0;
  m_bcs={bc1,bc2,bc3,bc3};
  m_tag=tag;
  pack();
}

Trigger::~Trigger(){
  m_bcs.clear();
}

Trigger * Trigger::Clone(){
  return new Trigger(this);
}

string Trigger::ToString(){
  ostringstream os;
  switch(m_pattern){
  case Trigger_01: os << "Trigger_01"; break;
  case Trigger_02: os << "Trigger_02"; break;
  case Trigger_03: os << "Trigger_03"; break;
  case Trigger_04: os << "Trigger_04"; break;
  case Trigger_05: os << "Trigger_05"; break;
  case Trigger_06: os << "Trigger_06"; break;
  case Trigger_07: os << "Trigger_07"; break;
  case Trigger_08: os << "Trigger_08"; break;
  case Trigger_09: os << "Trigger_09"; break;
  case Trigger_10: os << "Trigger_10"; break;
  case Trigger_11: os << "Trigger_11"; break;
  case Trigger_12: os << "Trigger_12"; break;
  case Trigger_13: os << "Trigger_13"; break;
  case Trigger_14: os << "Trigger_14"; break;
  case Trigger_15: os << "Trigger_15"; break;
  default:         os << "NoTrigger";  break;
  }
  os << " (0x" << hex << m_pattern << dec << ")"
     << " Data_" << m_tag 
     << " (0x" << hex << m_data2symbol[m_tag] << dec << ")";
  return os.str();
}

uint32_t Trigger::UnPack(uint8_t * bytes, uint32_t maxlen){
  switch(bytes[0]){
  case Trigger_01:
  case Trigger_02:
  case Trigger_03:
  case Trigger_04:
  case Trigger_05:
  case Trigger_06:
  case Trigger_07:
  case Trigger_08:
  case Trigger_09:
  case Trigger_10:
  case Trigger_11:
  case Trigger_12:
  case Trigger_13:
  case Trigger_14:
  case Trigger_15:
    break;
  default:
    return 0;
  }
  if(maxlen<2) return 0;
  m_pattern=bytes[0];
  m_tag=m_symbol2data[bytes[1]];
  return 2; 
}

uint32_t Trigger::Pack(uint8_t * bytes){
  bytes[0]=m_pattern&0xFF;
  bytes[1]=m_data2symbol[m_tag&0xFF];
  return 2;
}

uint32_t Trigger::GetType(){
  return Command::TRIGGER;
}

void Trigger::pack(){
  if     (!m_bcs[0] && !m_bcs[1] && !m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_01; }
  else if(!m_bcs[0] && !m_bcs[1] &&  m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_02; }
  else if(!m_bcs[0] && !m_bcs[1] &&  m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_03; }
  else if(!m_bcs[0] &&  m_bcs[1] && !m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_04; }
  else if(!m_bcs[0] &&  m_bcs[1] && !m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_05; }
  else if(!m_bcs[0] &&  m_bcs[1] &&  m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_06; }
  else if(!m_bcs[0] &&  m_bcs[1] &&  m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_07; }
  else if( m_bcs[0] && !m_bcs[1] && !m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_08; }
  else if( m_bcs[0] && !m_bcs[1] && !m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_09; }
  else if( m_bcs[0] && !m_bcs[1] &&  m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_10; }
  else if( m_bcs[0] && !m_bcs[1] &&  m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_11; }
  else if( m_bcs[0] &&  m_bcs[1] && !m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_12; }
  else if( m_bcs[0] &&  m_bcs[1] && !m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_13; }
  else if( m_bcs[0] &&  m_bcs[1] &&  m_bcs[2] && !m_bcs[3]){ m_pattern=Trigger_14; }
  else if( m_bcs[0] &&  m_bcs[1] &&  m_bcs[2] &&  m_bcs[3]){ m_pattern=Trigger_15; }
}

void Trigger::unpack(){
  switch(m_pattern){
  case Trigger_01: m_bcs[0]=0; m_bcs[1]=0; m_bcs[2]=0; m_bcs[3]=1; break;
  case Trigger_02: m_bcs[0]=0; m_bcs[1]=0; m_bcs[2]=1; m_bcs[3]=0; break;
  case Trigger_03: m_bcs[0]=0; m_bcs[1]=0; m_bcs[2]=1; m_bcs[3]=1; break;
  case Trigger_04: m_bcs[0]=0; m_bcs[1]=1; m_bcs[2]=0; m_bcs[3]=0; break;
  case Trigger_05: m_bcs[0]=0; m_bcs[1]=1; m_bcs[2]=0; m_bcs[3]=1; break;
  case Trigger_06: m_bcs[0]=0; m_bcs[1]=1; m_bcs[2]=1; m_bcs[3]=0; break;
  case Trigger_07: m_bcs[0]=0; m_bcs[1]=1; m_bcs[2]=1; m_bcs[3]=1; break;
  case Trigger_08: m_bcs[0]=1; m_bcs[1]=0; m_bcs[2]=0; m_bcs[3]=0; break;
  case Trigger_09: m_bcs[0]=1; m_bcs[1]=0; m_bcs[2]=0; m_bcs[3]=1; break;
  case Trigger_10: m_bcs[0]=1; m_bcs[1]=0; m_bcs[2]=1; m_bcs[3]=0; break;
  case Trigger_11: m_bcs[0]=1; m_bcs[1]=0; m_bcs[2]=1; m_bcs[3]=1; break;
  case Trigger_12: m_bcs[0]=1; m_bcs[1]=1; m_bcs[2]=0; m_bcs[3]=0; break;
  case Trigger_13: m_bcs[0]=1; m_bcs[1]=1; m_bcs[2]=0; m_bcs[3]=1; break;
  case Trigger_14: m_bcs[0]=1; m_bcs[1]=1; m_bcs[2]=1; m_bcs[3]=0; break;
  case Trigger_15: m_bcs[0]=1; m_bcs[1]=1; m_bcs[2]=1; m_bcs[3]=1; break;
  default:         m_bcs[0]=0; m_bcs[1]=0; m_bcs[2]=0; m_bcs[3]=0; break;
  }
}

void Trigger::SetPattern(uint32_t pattern){
  m_pattern = pattern;
  unpack();
}

uint32_t Trigger::GetPattern(){
  return m_pattern;
}

void Trigger::SetTag(uint32_t tag){
  m_tag=tag;
}

uint32_t Trigger::GetTag(){
  return m_tag;
}

void Trigger::SetTrigger(uint32_t bc, bool enable){
  if(bc>3){return;}
  m_bcs[bc]=enable;
  pack();
}

bool Trigger::GetTrigger(uint32_t bc){
  if(bc>3){return false;}
  return m_bcs[bc];
}

