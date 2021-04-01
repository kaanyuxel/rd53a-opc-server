#This file was automatically generated
#on 23/03/2021 at 16:16:35

add_library(netio OBJECT 
            src/netio.cpp
            src/config.h.in
            src/serialization.cpp
            src/serialization.hpp
            src/deserialization.hpp
            src/fi_verbs.hpp
            src/posix.cpp
            src/posix.hpp
            src/sockcfg.cpp
            src/endpoint.cpp
            src/message.cpp
            src/eventloop.cpp
            src/backend.cpp
            src/backend.hpp
            src/sockets.cpp
            src/context.cpp
            src/buffer.cpp
            src/utility.hpp
           )

target_include_directories(netio PUBLIC .)
#Additional customizations
target_include_directories(netio PUBLIC $ENV{TBB__HOME}/include)
target_link_directories(netio PUBLIC $ENV{TBB__HOME}/lib)
target_link_libraries(netio PUBLIC tbb)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
