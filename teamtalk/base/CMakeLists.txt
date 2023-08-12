cmake_minimum_required(VERSION 3.12)
PROJECT(Base)


AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(../pb/protocol SRC_LIST)
AUX_SOURCE_DIRECTORY(../pkg/include/base SRC_LIST)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/src/pkg/lib)
set(INSTALL_HEADER_PATH ${CMAKE_SOURCE_DIR}/pub/include)
set(INSTALL_LIB_PATH ${CMAKE_SOURCE_DIR}/pub/lib)

message(STATUS include_dir:${LIBRARY_HEADER_PATH})
message(STATUS lib_dir:${LIBRARY_LIB_PATH})


ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON)

set(CMAKE_CXX_STANDARD 11)




ADD_LIBRARY(base STATIC ${SRC_LIST})

TARGET_LINK_LIBRARIES(base pthread crypto ssl protobuf jsoncpp spdlog)

target_include_directories(
        base PRIVATE
        .
        ../pb/protocol
        ../pkg/include/base
        /usr/local/include
)

target_link_directories(
        base PRIVATE
        /usr/local/lib
)
# install headerss
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/pkg/include/base
        DESTINATION ${INSTALL_HEADER_PATH}
        FILES_MATCHING PATTERN "*.h")
#install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${CMAKE_PROJECT_NAME})
install(TARGETS base DESTINATION ${INSTALL_LIB_PATH} EXPORT libase)
