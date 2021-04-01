#include "RD53Emulator/SensorScan.h"

#include <iostream>
#include <chrono>
#include <TH1F.h>

using namespace std;
using namespace RD53A;

SensorScan::SensorScan(){
  m_continue = false;
  m_verbose = false;

}

SensorScan::~SensorScan(){}

void SensorScan::Run(){

  std::map<std::string,TH1F*> ntc1;
  std::map<std::string,TH1F*> ntc2;
  std::map<std::string,TH1F*> ntc3;
  std::map<std::string,TH1F*> ntc4;
  std::map<std::string,TH1F*> bjt1;
  std::map<std::string,TH1F*> bjt2;
  std::map<std::string,TH1F*> bjt3;
  std::map<std::string,TH1F*> bjt4;

  //create histograms
  for(auto fe : GetFEs()){
	ntc1[fe->GetName()] = new TH1F(("ntc1_"+fe->GetName()).c_str(),"NTC1;Temperature [C]", 1000, -50, 50);
	ntc2[fe->GetName()] = new TH1F(("ntc2_"+fe->GetName()).c_str(),"NTC2;Temperature [C]", 1000, -50, 50);
	ntc3[fe->GetName()] = new TH1F(("ntc3_"+fe->GetName()).c_str(),"NTC3;Temperature [C]", 1000, -50, 50);
	ntc4[fe->GetName()] = new TH1F(("ntc4_"+fe->GetName()).c_str(),"NTC4;Temperature [C]", 1000, -50, 50);
	bjt1[fe->GetName()] = new TH1F(("bjt1_"+fe->GetName()).c_str(),"BJT1;TID [Rad]", 1000, 0, 500);
	bjt2[fe->GetName()] = new TH1F(("bjt2_"+fe->GetName()).c_str(),"BJT2;TID [Rad]", 1000, 0, 500);
	bjt3[fe->GetName()] = new TH1F(("bjt3_"+fe->GetName()).c_str(),"BJT3;TID [Rad]", 1000, 0, 500);
	bjt4[fe->GetName()] = new TH1F(("bjt4_"+fe->GetName()).c_str(),"BJT4;TID [Rad]", 1000, 0, 500);
   }

  Loop();

  for(auto fe : GetFEs()){
	ntc1[fe->GetName()]->Fill(fe->GetTemperatureSensor(0)->GetTemperature());
	ntc2[fe->GetName()]->Fill(fe->GetTemperatureSensor(1)->GetTemperature());
	ntc3[fe->GetName()]->Fill(fe->GetTemperatureSensor(2)->GetTemperature());
	ntc4[fe->GetName()]->Fill(fe->GetTemperatureSensor(3)->GetTemperature());
    bjt1[fe->GetName()]->Fill(fe->GetRadiationSensor(0)->GetADC());
    bjt2[fe->GetName()]->Fill(fe->GetRadiationSensor(1)->GetADC());
    bjt3[fe->GetName()]->Fill(fe->GetRadiationSensor(2)->GetADC());
    bjt4[fe->GetName()]->Fill(fe->GetRadiationSensor(3)->GetADC());
  }

  //Save the results
  for(auto fe : GetFEs()){
    ntc1[fe->GetName()]->Write();
    ntc2[fe->GetName()]->Write();
    ntc3[fe->GetName()]->Write();
    ntc4[fe->GetName()]->Write();
    bjt1[fe->GetName()]->Write();
    bjt2[fe->GetName()]->Write();
    bjt3[fe->GetName()]->Write();
    bjt4[fe->GetName()]->Write();
  }
}

void SensorScan::Loop(){

  //Init the ADCS
  for(auto fe : GetFEs()){
    fe->InitAdc();
    Send(fe);
    if(m_verbose)
      cout << __PRETTY_FUNCTION__ << "ADC has been initialized." << endl;
  }

  m_continue = true;
  //Keep looping
  int sensor_id = 0;
  bool read_radiation_sensors = false;
  while(m_continue){

    for(auto fe : GetFEs()){
      //Power on sensors
      if(fe->GetTemperatureSensor(sensor_id)->GetPower() == false && fe->GetRadiationSensor(sensor_id)->GetPower() == false){
        fe->ReadSensor(sensor_id, read_radiation_sensors);
        Send(fe);
        if(m_verbose){
          auto end = std::chrono::system_clock::now();
          cout << __PRETTY_FUNCTION__ << "Sensor : " << sensor_id << " Command Send Time : " << std::chrono::system_clock::to_time_t(end) << " Read Radiation Sensor Status : " << read_radiation_sensors << endl;
        }
      }

      //Read Temperature Sensors and Fill Histograms
      if(fe->GetTemperatureSensor(sensor_id)->isUpdated() == true){
    	if(m_verbose){
    	  auto end = std::chrono::system_clock::now();
    	  cout << __PRETTY_FUNCTION__<< " Read Temperature Sensor Time : " << std::chrono::system_clock::to_time_t(end) << " Sensor : " << sensor_id << " ADC Value : " << fe->GetTemperatureSensor(sensor_id)->GetADC() << endl;
    	}
        fe->GetTemperatureSensor(sensor_id)->Update(false);
        fe->GetTemperatureSensor(sensor_id)->SetPower(false);
        sensor_id++;
    	if(sensor_id == 4){
    	  sensor_id = 0;
          read_radiation_sensors = true;
    	}
      }

      //Read Radiation Sensors and Fill Histograms
      if(fe->GetRadiationSensor(sensor_id)->isUpdated() == true){
    	if(m_verbose){
          auto end = std::chrono::system_clock::now();
          cout << __PRETTY_FUNCTION__<< "Read Radiation Sensor Time : " << std::chrono::system_clock::to_time_t(end) << " Sensor : " << sensor_id << " ADC Value : " << fe->GetRadiationSensor(sensor_id)->GetADC() << endl;
    	}
        fe->GetRadiationSensor(sensor_id)->Update(false);
        fe->GetRadiationSensor(sensor_id)->SetPower(false);
        sensor_id++;
    	if(sensor_id == 4){
    	  sensor_id = 0;
          read_radiation_sensors = false;
          m_continue = false;
    	}
      }
   }
 }
}

void SensorScan::SetVerbose(bool enable){
  m_verbose = enable;
}
