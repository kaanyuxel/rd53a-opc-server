#include "RD53Emulator/RadiationSensor.h"

using namespace RD53A;

RadiationSensor::RadiationSensor(){
  m_adc=0;
  m_value=0;
  m_power=false;
  m_updated=false;
}

RadiationSensor::~RadiationSensor(){}

void RadiationSensor::SetADC(uint32_t value){
  m_adc=value;
}

void RadiationSensor::SetValue(float value){
  m_value=value;
}

uint32_t RadiationSensor::GetADC(){
  return m_adc;
}

float RadiationSensor::GetValue(){
  return m_value;
}

void RadiationSensor::SetPower(bool power){
  m_power=power;
}

bool RadiationSensor::GetPower(){
  return m_power;
}

void RadiationSensor::Update(bool status){
  m_updated=status;
}

bool RadiationSensor::isUpdated(){
  return m_updated;
}
