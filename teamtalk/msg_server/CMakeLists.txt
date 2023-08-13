cmake_minimum_required(VERSION 3.10)
PROJECT(msgserver)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)

SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/public/bin)

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)

ADD_EXECUTABLE(msg_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(msg_server base protobuf ssl crypto pthread spdlog)

target_include_directories(
        msg_server PRIVATE
        .
        ../base/include
        ../pb/protocol
        /usr/local/include
)

target_link_directories(
        msg_server PRIVATE
        .
        /usr/local/lib
        ../base/lib
)