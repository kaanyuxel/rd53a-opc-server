# ----------------------------------------------------
## Please see Documentation/quasarBuildSystem.html for
## information how to use this file.
## ---------------------------------------------------

if(NOT DEFINED ENV{ROOT__HOME})
set(ENV{ROOT__HOME} $ENV{LCG_INST_PATH}/$ENV{TDAQ_LCG_RELEASE}/ROOT/v6.20.06/$ENV{CMTCONFIG})
message("ROOT__HOME=$ENV{ROOT__HOME}")
endif()
if(NOT DEFINED ENV{TBB__HOME})
set(ENV{TBB__HOME} $ENV{LCG_INST_PATH}/$ENV{TDAQ_LCG_RELEASE}/tbb/2020_U1/$ENV{CMTCONFIG})
message("TBB__HOME=$ENV{TBB__HOME}")
endif()
if(NOT DEFINED ENV{BOOST__HOME})
set(ENV{BOOST__HOME} $ENV{LCG_INST_PATH}/$ENV{TDAQ_LCG_RELEASE}/Boost/1.72.0/$ENV{CMTCONFIG})
message("BOOST__HOME=$ENV{BOOST__HOME}")
endif()

set(CUSTOM_SERVER_MODULES RD53Emulator netio)
set(EXECUTABLE OpcUaServer)
set(SERVER_INCLUDE_DIRECTORIES RD53Emulator netio)
set(SERVER_LINK_LIBRARIES tbb RIO Hist Core boost_program_options boost_system-mt)
set(SERVER_LINK_DIRECTORIES $ENV{TBB__HOME}/lib $ENV{ROOT__HOME}/lib $ENV{BOOST__HOME}/lib)

set(IGNORE_DEFAULT_BOOST_SETUP ON)

##
## If ON, in addition to an executable, a shared object will be created.
##
set(BUILD_SERVER_SHARED_LIB OFF)

##
## Add here any additional boost libraries needed with their canonical name
## examples: date_time atomic etc.
## Note: boost paths are resolved either from $BOOST_ROOT if defined or system paths as fallback
##
set(ADDITIONAL_BOOST_LIBS )
