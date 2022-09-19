cmake_minimum_required(VERSION 3.10)
PROJECT(routeserver)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)

 SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/pub/bin)


ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)




ADD_EXECUTABLE(route_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(route_server base protobuf pthread ssl crypto)

target_include_directories(
        route_server PRIVATE
        .
        ../pkg/include
        ../pb/protocol
        ../pkg/security
        /usr/local/include
)

target_link_directories(
        route_server PRIVATE
        .
        /usr/local/lib
        ../pkg/lib
)