cmake_minimum_required(VERSION 3.10)
PROJECT(msfs)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(../pb/protocol SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/public/bin)


ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)


ADD_EXECUTABLE(msfs ${SRC_LIST} )

TARGET_LINK_LIBRARIES(msfs base yaml-cpp)

target_include_directories(
        msfs PRIVATE
        .
        ../base/include
        /usr/local/include
        ${YAML_CPP_INCLUDE_DIR}
)

target_link_directories(
        msfs PRIVATE
        .
        /usr/local/lib
        ../base/lib
        ${YAML_CPP_LIB_DIR}
)
