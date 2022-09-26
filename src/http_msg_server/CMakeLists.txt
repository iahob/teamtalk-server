cmake_minimum_required(VERSION 3.10)
PROJECT(http_msg_server)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)

 SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/pub/bin)

if(APPLE)

endif()


ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)


ADD_EXECUTABLE(http_msg_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(http_msg_server base protobuf  pthread ssl crypto yaml-cpp)


target_include_directories(
        http_msg_server PRIVATE
        .
        ../pkg/include
        ../pb/protocol
        /usr/local/include
        ${YAML_CPP_INCLUDE_DIR}
         )

target_link_directories(
        http_msg_server PRIVATE
        .
        /usr/local/lib
        ../pkg/lib
        ${YAML_CPP_LIB_DIR}  
)