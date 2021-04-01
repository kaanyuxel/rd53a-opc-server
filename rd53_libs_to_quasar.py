#!/usr/bin/env python

import os
import sys
import time
import argparse

def file_replace(path, keyPair, backup=False):
    if backup: os.system("cp %s %s.%s.bak" % (path,path,time.strftime("%Y%m%d-%H%M%S")))
    print ("Open: %s" % path)
    fr = open(path)
    fc = []
    for line in fr.readlines():
        line=line.strip()
        for key in keyPair:
            if line==key:
                print ("Replace '%s' => '%s'" % (line,keyPair[key]))
                line=keyPair[key]
                break
            pass
        fc.append(line)
        pass
    fw = open(path,'w')
    for line in fc:
        fw.write(line+"\n")
        pass
    fw.close()
    pass


parser = argparse.ArgumentParser()
parser.add_argument("sw_dir",help="rd53 emulator path")
parser.add_argument("opc_project_dir",help="opc ua project path")
args = parser.parse_args()

files={"RD53Emulator":[
               "RD53Emulator/BCR.h",
               "RD53Emulator/BlankFrame.h",
               "RD53Emulator/Cal.h",
               "RD53Emulator/Command.h",
               "RD53Emulator/Configuration.h",
               "RD53Emulator/DataFrame.h",
               "RD53Emulator/Decoder.h",
               "RD53Emulator/ECR.h",
               "RD53Emulator/Emulator.h",
               "RD53Emulator/Encoder.h",
               "RD53Emulator/Field.h",
               "RD53Emulator/Frame.h",
               "RD53Emulator/FrontEnd.h",
               "RD53Emulator/Handler.h",
               "RD53Emulator/Hit.h",
               "RD53Emulator/Matrix.h",
               "RD53Emulator/NetioClient.h",
               "RD53Emulator/Noop.h",
               "RD53Emulator/RadiationSensor.h",
               "RD53Emulator/Pixel.h",
               "RD53Emulator/Pulse.h",
               "RD53Emulator/RdReg.h",
               "RD53Emulator/Register.h",
               "RD53Emulator/RegisterFrame.h",
               "RD53Emulator/RunNumber.h",
               "RD53Emulator/SensorScan.h",
               "RD53Emulator/Sync.h",
               "RD53Emulator/TemperatureSensor.h",
               "RD53Emulator/Trigger.h",
               "RD53Emulator/Tools.h",
               "RD53Emulator/WrReg.h",
               "src/BCR.cpp",
               "src/BlankFrame.cpp",
               "src/Cal.cpp",
               "src/Command.cpp",
               "src/Configuration.cpp",
               "src/DataFrame.cpp",
               "src/Decoder.cpp",
               "src/ECR.cpp",
               "src/Emulator.cpp",
               "src/Encoder.cpp",
               "src/Field.cpp",
               "src/Frame.cpp",
               "src/FrontEnd.cpp",
               "src/Handler.cpp",
               "src/Hit.cpp",
               "src/Matrix.cpp",
               "src/NetioClient.cpp",
               "src/Noop.cpp",
               "src/Pixel.cpp",
               "src/RadiationSensor.cpp",
               "src/Pulse.cpp",
               "src/RdReg.cpp",
               "src/Register.cpp",
               "src/RegisterFrame.cpp",
               "src/RunNumber.cpp",
               "src/SensorScan.cpp",
               "src/Sync.cpp",
               "src/TemperatureSensor.cpp",
               "src/Trigger.cpp",
               "src/Tools.cpp",
               "src/WrReg.cpp",],
    "netio":["netio/netio.hpp",
             "src/netio.cpp",
             "src/config.h.in",
             "src/serialization.cpp",
             "src/serialization.hpp",
             "src/deserialization.hpp",
             "src/fi_verbs.hpp",
             "src/posix.cpp",
             "src/posix.hpp",
             "src/sockcfg.cpp",
             "src/endpoint.cpp",
             "src/message.cpp",
             "src/eventloop.cpp",
             "src/backend.cpp",
             "src/backend.hpp",
             "src/sockets.cpp",
             "src/context.cpp",
             "src/buffer.cpp",
             "src/utility.hpp",
             ]             
      }

copts={"netio":['target_include_directories(netio PUBLIC $ENV{TBB__HOME}/include)',
                'target_link_directories(netio PUBLIC $ENV{TBB__HOME}/lib)',
                'target_link_libraries(netio PUBLIC tbb)',
                'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")'],
       "RD53Emulator":['target_include_directories(RD53Emulator PUBLIC $ENV{ROOT__HOME}/include)',
                       'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")'],
       }

extra={"netio/src/config.h":
       ['#pragma once',
        '#define VERSION_STRING "0.8.0"',
        ]
       }

downs={"RD53Emulator/json.hpp":"https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"}
        
print("Check OPC-UA project exists")
if not os.path.exists(args.opc_project_dir):
    print ("Project does not exist: %s" % args.opc_project_dir)
    sys.exit()
    pass

print("Check source files exist")
for pkg in files:
    print(pkg)
    for f in files[pkg]:
        fp="%s/%s/%s" % (args.sw_dir,pkg,f)
        if not os.path.exists(fp):
            print(fp)
            print ("File does not exist: %s" % f)
            sys.exit()
            pass
        pass
    pass

print("Copy files to OPC-UA project")
for pkg in files:
    for f in files[pkg]:
        src="%s/%s/%s" % (args.sw_dir,pkg,f)
        dst="%s/%s/%s" % (args.opc_project_dir,pkg,f)
        if not os.path.exists(os.path.dirname(dst)):
            os.system("mkdir -p %s" % os.path.dirname(dst))
            pass
        os.system("cp -v %s %s" % (src,dst))
        pass
    pass

print("Extra files")
for f in extra:
    fw=open("%s/%s" % (args.opc_project_dir,f),"w+")
    for line in extra[f]:
        fw.write(line+"\n")
        pass
    fw.close()
    pass

print("Download files")
for f in downs:
    cmd="curl -Lo %s/%s %s" % (args.opc_project_dir,f,downs[f])
    print(cmd)
    os.system(cmd)
    pass

print("Create CMakeLists.txt files")
for pkg in files:
    print("File: %s/%s/CMakeLists.txt" % (args.opc_project_dir,pkg))
    ttt ="#This file was automatically generated\n"
    ttt+="#on %s\n" % time.strftime("%d/%m/%Y at %H:%M:%S")
    ttt+="\n"
    ttt+="add_library(%s OBJECT \n" % pkg
    for f in files[pkg]:
        if not "src" in f: continue
        ttt+="            %s\n" % f
        pass
    ttt+="           )\n"
    ttt+="\n"
    ttt+="target_include_directories(%s PUBLIC .)\n" % pkg
    if pkg in copts:
        ttt+="#Additional customizations\n"
        for line in copts[pkg]:
            ttt+=line+"\n"
            pass
        pass
    fw=open("%s/%s/CMakeLists.txt" % (args.opc_project_dir,pkg),"w+")
    fw.write(ttt)
    fw.close()
    pass

print("Update the OPC-UA project settings")

file_replace("%s/ProjectSettings.cmake" % args.opc_project_dir,
             {"set(CUSTOM_SERVER_MODULES )":"set(CUSTOM_SERVER_MODULES %s)" % " ".join(files.keys()),
              "set(SERVER_INCLUDE_DIRECTORIES )":"set(SERVER_INCLUDE_DIRECTORIES %s)" % " ".join(files.keys())}
             )

print("Have a nice day")

