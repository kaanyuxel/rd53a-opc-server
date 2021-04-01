#include "RD53Emulator/Decoder.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace RD53A;

Decoder::Decoder(){
  m_bytes.reserve(1000000);
  m_bytes.resize(1000000,0);
  m_length = 0;
  m_fD=new DataFrame();
  m_fR=new RegisterFrame();
  m_fB=new BlankFrame();
}

Decoder::~Decoder(){
  m_bytes.clear();
  ClearFrames();
  delete m_fD;
  delete m_fR;
  delete m_fB;
}

void Decoder::AddBytes(uint8_t *bytes, uint32_t pos, uint32_t len){
  for(uint32_t i=pos;i<pos+len;i++){
    m_bytes[m_length]=bytes[i];
    m_length++;
  }
}

void Decoder::SetBytes(uint8_t *bytes, uint32_t len){
  m_length=0;
  for(uint32_t i=0;i<len;i++){
    m_bytes[m_length]=bytes[i];
    m_length++;
  }
}

void Decoder::AddFrame(Frame *frame){
  m_frames.push_back(frame);
}
  
void Decoder::ClearBytes(){
  m_length=0;
}
  
void Decoder::ClearFrames(){
  while(!m_frames.empty()){
    Frame* frame = m_frames.back();
    delete frame;
    m_frames.pop_back();
  }
}

void Decoder::Clear(){
  ClearBytes();
  ClearFrames();
}
  
string Decoder::GetByteString(){
  ostringstream os;
  os << hex; 
  for(uint32_t pos=0; pos<m_length; pos++){
    os << setw(2) << setfill('0') << (uint32_t) m_bytes[pos] << " ";
  }
  return os.str();
}

uint8_t * Decoder::GetBytes(){
  return m_bytes.data();
}
  
uint32_t Decoder::GetLength(){
  return m_length;
}
  
vector<Frame*> & Decoder::GetFrames(){
  return m_frames;
}

void Decoder::Encode(){
  uint32_t pos=0;
  for(uint32_t i=0;i<m_frames.size();i++){
    pos+=m_frames[i]->Pack(&m_bytes[pos]);
  }
  m_length=pos;
}

void Decoder::Decode(const bool verbose){
  ClearFrames();
  uint32_t pos=0;
  uint32_t nb=0;
  uint32_t tnb=m_length;
  while(pos!=tnb){
    //inspect the symbol
    Frame * frame=0;
    if     ((nb=m_fR->UnPack(&m_bytes[pos],tnb-pos))>0){
      if(verbose) cout << "Decoder::Decode() new register frame" << endl;
      frame=m_fR; m_fR=new RegisterFrame();
    }
    else if((nb=m_fB->UnPack(&m_bytes[pos],tnb-pos))>0){
      if(verbose) cout << "Decoder::Decode() new blank frame" << endl;
      frame=m_fB; m_fB=new BlankFrame();
    }
    else if((nb=m_fD->UnPack(&m_bytes[pos],tnb-pos))>0){
      if(verbose) cout << "Decoder::Decode() new data frame" << endl;
      frame=m_fD; m_fD=new DataFrame();
    }
    else{
      cout << __PRETTY_FUNCTION__ << "Cannot decode byte sequence: "
           << "0x" << hex << setw(2) << setfill('0') << (uint32_t) m_bytes[pos] << dec 
           << " at index: " << pos 
           << " ...skipping" << endl;
      pos++;
      continue;
    }
    if(!frame){pos++; continue;}
    m_frames.push_back(frame);
    pos+=nb;
  }
  //Check if there is bytes left
}
