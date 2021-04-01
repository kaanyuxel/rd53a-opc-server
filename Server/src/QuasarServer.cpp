/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * QuasarServer.cpp
 *
 *  Created on: Nov 6, 2015
 * 		Author: Damian Abalo Miron <damian.abalo@cern.ch>
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <thread>

#include "QuasarServer.h"
#include <LogIt.h>
#include <shutdown.h>
#include <DRoot.h>
#include <DRD53A.h>
#include <ASRD53A.h>

#include "RD53Emulator/SensorScan.h"
#include "RD53Emulator/Handler.h"
#include "RD53Emulator/FrontEnd.h"

using namespace RD53A;

QuasarServer::QuasarServer() : BaseQuasarServer()
{

}

QuasarServer::~QuasarServer()
{
 
}

void QuasarServer::mainLoop()
{
    printServerMsg("Press "+std::string(SHUTDOWN_SEQUENCE)+" to shutdown server");

    // Wait for user command to terminate the server thread.

    // Create a SensorScan Handler
    SensorScan *scan = new SensorScan();
    // Add each RD53A to the Sensor Scan

    LOG(Log::INF) << "Load RD53As" ;
    for(Device::DRD53A *rd53a : Device::DRoot::getInstance()->rd53as()){
      LOG(Log::INF) << " Name: " <<rd53a->getFullName()
    		        << " Host: " << rd53a->getAddressSpaceLink()->getHost().toUtf8()
                    << " CmdPort: " << rd53a->getAddressSpaceLink()->getCmdPort()
                    << " DataPort: " << rd53a->getAddressSpaceLink()->getDataPort()
                    << " CmdElink: " << rd53a->getAddressSpaceLink()->getCmdElink()
                    << " DataElink: " << rd53a->getAddressSpaceLink()->getDataElink();

     scan->AddMapping(rd53a->getFullName(),
    		 	 	  rd53a->getFullName() + ".json",
                      rd53a->getAddressSpaceLink()->getCmdElink(),
                      rd53a->getAddressSpaceLink()->getDataElink(),
                      rd53a->getAddressSpaceLink()->getHost().toUtf8(),
                      rd53a->getAddressSpaceLink()->getCmdPort(),
                      rd53a->getAddressSpaceLink()->getHost().toUtf8(),
                      rd53a->getAddressSpaceLink()->getDataPort());

      scan->AddFE(rd53a->getFullName());
      scan->Connect();
      scan->Config();
    }

    // Run the scan in a different thread

    while(ShutDownFlag() == 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      // Write the values 
      scan->Loop();
      
      for(Device::DRD53A *rd53a : Device::DRoot::getInstance()->rd53as()){
        RD53A::FrontEnd *fe = scan->GetFE(rd53a->getFullName());
        //Fill in the results
        rd53a->getAddressSpaceLink()->setTemp_1(fe->GetTemperatureSensor(0)->GetTemperature(),OpcUa_Good);
        rd53a->getAddressSpaceLink()->setTemp_2(fe->GetTemperatureSensor(1)->GetTemperature(),OpcUa_Good);
        rd53a->getAddressSpaceLink()->setTemp_3(fe->GetTemperatureSensor(2)->GetTemperature(),OpcUa_Good);
        rd53a->getAddressSpaceLink()->setTemp_4(fe->GetTemperatureSensor(3)->GetTemperature(),OpcUa_Good);
        //LOG(Log::INF) << fe->GetTemperatureSensor(0)->GetTemperature() << ", " << fe->GetTemperatureSensor(1)->GetTemperature() << ", " << fe->GetTemperatureSensor(2)->GetTemperature() ;
        rd53a->getAddressSpaceLink()->setRad_1(fe->GetRadiationSensor(0)->GetADC(),OpcUa_Good);
        rd53a->getAddressSpaceLink()->setRad_2(fe->GetRadiationSensor(1)->GetADC(),OpcUa_Good);
        rd53a->getAddressSpaceLink()->setRad_3(fe->GetRadiationSensor(2)->GetADC(),OpcUa_Good);
        rd53a->getAddressSpaceLink()->setRad_4(fe->GetRadiationSensor(3)->GetADC(),OpcUa_Good);
      }
    }

    // Stop the scan
    scan->Disconnect();

    // Delete the sensor scan
    delete scan;
    printServerMsg(" Shutting down server");
}

void QuasarServer::initialize()
{
    LOG(Log::INF) << "Initializing Quasar server.";

}

void QuasarServer::shutdown()
{
	LOG(Log::INF) << "Shutting down Quasar server.";
}

void QuasarServer::initializeLogIt()
{
	BaseQuasarServer::initializeLogIt();
    LOG(Log::INF) << "Logging initialized.";
}
