#include "RD53Emulator/TemperatureSensor.h"
#include <math.h>

using namespace RD53A;
using namespace std;

TemperatureSensor::TemperatureSensor(){
  m_adc=0;
  m_value=0;
  m_power=false;
  m_updated=false;
  m_voltage=0.0;
  m_temperature=0.0;
  m_calibration=1.24;
}

TemperatureSensor::~TemperatureSensor(){}

void TemperatureSensor::SetADC(uint32_t value){
  m_adc=value;
}

void TemperatureSensor::SetValue(float value){
  m_value=value;
}

uint32_t TemperatureSensor::GetADC(){
  return m_adc;
}

float TemperatureSensor::GetValue(){
  return m_value;
}

void TemperatureSensor::SetPower(bool power){
  m_power=power;
}

bool TemperatureSensor::GetPower(){
  return m_power;
}

void TemperatureSensor::Update(bool status){
  m_updated=status;
}

bool TemperatureSensor::isUpdated(){
  return m_updated;
}

void TemperatureSensor::SetCalibration(float cal){
   m_calibration = cal;
}

float TemperatureSensor::GetCalibration(){
   return m_calibration;
}

float TemperatureSensor::GetTemperature(){
  m_voltage = (slope_adc*m_adc + offset_adc)/1000.0;
  m_temperature = e_charge / ( m_calibration * k_boltzman * log(15)) * m_voltage - 273.15;
  return m_temperature;
}
