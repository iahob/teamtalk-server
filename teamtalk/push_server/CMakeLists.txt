cmake_minimum_required(VERSION 3.10)
PROJECT(pushserver)

AUX_SOURCE_DIRECTORY(./ SRC_LIST1)
AUX_SOURCE_DIRECTORY(./socket SRC_LIST2)
AUX_SOURCE_DIRECTORY(../pb/protocol SRC_LIST2)


SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/pub/bin)

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)

#INCLUDE_DIRECTORIES(./ ../pb/protocol ${PB_INCLUDE_DIR} ../pkg/slog ${OPENSSL_INCLUDE_DIR})
#LINK_DIRECTORIES(./ ${PB_LIB_DIR} ${SLOG_LIB_DIR} ${OPENSSL_LIB_DIR})


ADD_EXECUTABLE(push_server ${SRC_LIST1} ${SRC_LIST2}
        ../tools/daeml.cpp)

TARGET_LINK_LIBRARIES(push_server pthread protobuf ssl)

target_include_directories(
        push_server PRIVATE
        .
        ../pkg/include
        ../pb/protocol
        ../pkg/security
        /usr/local/include
)

target_link_directories(
        push_server PRIVATE
        .
        /usr/local/lib
        ../pkg/lib
)