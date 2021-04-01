#include "RD53Emulator/Pixel.h"

using namespace std;
using namespace RD53A;

Pixel::Pixel(uint8_t type){
  m_type=type;
  m_data=0;
}

Pixel::~Pixel(){}

uint8_t Pixel::GetType(){
  return m_type;
}

void Pixel::SetType(uint8_t type){
  m_type=type;
}

void Pixel::SetValue(uint32_t value){
  m_data = value;
}

uint32_t Pixel::GetValue(){
  return m_data;
}

void Pixel::SetValue(std::string name, uint32_t value){
  if     (name=="Enable") SetEnable(value);
  else if(name=="Inject") SetInject(value);
  else if(name=="InjEn")  SetInject(value);
  else if(name=="Hitbus") SetHitbus(value);
  else if(name=="TDAC")   SetTDAC(value);
  else if(name=="Gain")   SetGain(value);
}

uint32_t Pixel::GetValue(std::string name){
  if     (name=="Enable") return GetEnable();
  else if(name=="Inject") return GetInject();
  else if(name=="InjEn")  return GetInject();
  else if(name=="Hitbus") return GetHitbus();
  else if(name=="TDAC")   return GetTDAC();
  else if(name=="Gain")   return GetGain();
  return 0;
}

bool Pixel::GetEnable(){
  return ( m_data >> Pixel::Enable ) & 0x1;
}

void Pixel::SetEnable(bool enable){
  if (enable) m_data |= 1 << Pixel::Enable;
  else m_data &= ~ (1 << Pixel::Enable);
}

bool Pixel::GetInject(){
  return ( m_data >> Pixel::Inject ) & 0x1;
}

void Pixel::SetInject(bool enable){
  if (enable) m_data |= 1 << Pixel::Inject;
  else m_data &= ~ (1 << Pixel::Inject);
}

bool Pixel::GetHitbus(){
  return ( m_data >> Pixel::Hitbus ) & 0x1;
}

void Pixel::SetHitbus(bool enable){
  if (enable) m_data |= 1 << Pixel::Hitbus;
  else m_data &= ~ (1 << Pixel::Hitbus);
}

uint32_t Pixel::GetTDAC(){
  return ( m_data >> Pixel::TDAC ) & 0xF;
}

void Pixel::SetTDAC(uint32_t tdac){
  m_data &= ~(0xF << Pixel::TDAC);
  m_data |=  (tdac & 0xF) << Pixel::TDAC;
}

bool Pixel::GetGain(){
  return ( m_data >> Pixel::Gain ) & 0x1;
}

void Pixel::SetGain(bool enable){
  if (enable) m_data |= 1 << Pixel::Gain;
  else m_data &= ~ (1 << Pixel::Gain);
}
