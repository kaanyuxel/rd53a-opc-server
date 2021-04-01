#This file was automatically generated
#on 23/03/2021 at 16:16:35

add_library(RD53Emulator OBJECT 
            src/BCR.cpp
            src/BlankFrame.cpp
            src/Cal.cpp
            src/Command.cpp
            src/Configuration.cpp
            src/DataFrame.cpp
            src/Decoder.cpp
            src/ECR.cpp
            src/Emulator.cpp
            src/Encoder.cpp
            src/Field.cpp
            src/Frame.cpp
            src/FrontEnd.cpp
            src/Handler.cpp
            src/Hit.cpp
            src/Matrix.cpp
            src/NetioClient.cpp
            src/Noop.cpp
            src/Pixel.cpp
            src/RadiationSensor.cpp
            src/Pulse.cpp
            src/RdReg.cpp
            src/Register.cpp
            src/RegisterFrame.cpp
            src/RunNumber.cpp
            src/SensorScan.cpp
            src/Sync.cpp
            src/TemperatureSensor.cpp
            src/Trigger.cpp
            src/Tools.cpp
            src/WrReg.cpp
           )

target_include_directories(RD53Emulator PUBLIC .)
#Additional customizations
target_include_directories(RD53Emulator PUBLIC $ENV{ROOT__HOME}/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
