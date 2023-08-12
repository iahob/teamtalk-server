cmake_minimum_required(VERSION 3.10)
PROJECT(Server)
message(STATUS CMAKE_SOURCE_DIR:"${CMAKE_SOURCE_DIR}")
message(STATUS CMAKE_MAKE_PROGRAM:"${CMAKE_MAKE_PROGRAM}" )
message(STATUS CMAKE_COMMAND:"${CMAKE_COMMAND}" )
message(STATUS CMAKE_CURRENT_BINARY_DIR:"${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS CMAKE_CURRENT_SOURCE_DIR:"${CMAKE_CURRENT_SOURCE_DIR}")



ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
        -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
        -DAC_HAS_DEBUG -DLINUX_DAEMON)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_C_COMPILER /usr/local/Cellar/llvm/13.0.1_1/bin/clang-13)
#set(CMAKE_CXX_COMPILER /usr/local/Cellar/llvm/13.0.1_1/bin/clang-13)
#set(CMAKE_MAKE_PROGRAM /usr/local/Cellar/ninja/1.10.2_1/bin)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/module)
include(hiredis)
include(protobuf)
include(openssl)
include(spdlog)
include(jsoncpp)
include(curl)
include(yaml-cpp)
add_subdirectory(teamtalk)
