# RD53A OPC server
This OPC server reads RD53A registers from FELIX through NETIO, based on the Quasar framework.
The NETIO library is contained in the netio folder and compiled as part of the server.
RD53Emulator classes are used to describe the RD53A.
This also provides the encoder and decoder classes needed to send commands and read data out from the RD53A.
The description of the RD53A is based on the parameters described in Design/Design.xml.

 - Host: FELIX host name
 - Command Port: FELIX command port
 - Data Port: FELIX data port
 - Command Elink: FELIX command e-link
 - Data Elink: FELIX data e-link

# Requirements
This server requires Python3, Boost, C++17, Xerces-C, TBB, and ROOT.
They are provided as part of the LCG release.
To ease in the compilation, the standard TDAQ setup can be used to setup the environment.
Additional environmental variables ITK_PATH and ITK_DATA_PATH are required to find the configuration files for the RD53A. 

## Project settings
The following changes have been made to the `ProjectSettings.cmake` file: 
 - `set(ENV{ROOT__HOME} $ENV{LCG_INST_PATH}/$ENV{TDAQ_LCG_RELEASE}/ROOT/v6.20.06/$ENV{CMTCONFIG})`
 - `set(ENV{TBB__HOME} $ENV{LCG_INST_PATH}/$ENV{TDAQ_LCG_RELEASE}/tbb/2020_U1/$ENV{CMTCONFIG})`
 - `set(ENV{BOOST__HOME} $ENV{LCG_INST_PATH}/$ENV{TDAQ_LCG_RELEASE}/Boost/1.72.0/$ENV{CMTCONFIG})`
 - `set(SERVER_LINK_DIRECTORIES $ENV{TBB__HOME}/lib $ENV{ROOT__HOME}/lib $ENV{BOOST__HOME}/lib)`
 
# How to install OPC server 

If you are working on `lxplus`, you should source setup file. Then, it easy to install the OPC-UA server. Here are the commands that you need the follow;

```
source setup.sh
./quasar build
```
 
# How to test this OPC server
1. Modify the bin/config.xml to describe as many RD53A modules as you want.
```
<RD53A name="Emu-1" Host="localhost" CmdPort="12350" DataPort="12360" CmdElink="0" DataElink="0" ></RD53A>
```
2. Run the OPC server
This will send the appropriate configuration commands to FELIX, and receive the data from it.
Data is processed and made available through OPC server variables of radiation and temperature.
```
cd build/bin
./OpcUaServer config.xml
```
3. Run the OPC client
This allows the user to read the server variables using the OPC protocol.
```
cd share/bin
./rd53a_opc_client.py -n opc.tcp://localhost:4841
```

# Authors
  - Kaan Yuksel Oyulmaz
  - Carlos Solans
