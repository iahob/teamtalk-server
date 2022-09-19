cmake_minimum_required(VERSION 3.10)
PROJECT(security)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(../pkg/include/security SRC_LIST)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/src/pkg/lib)
set(INSTALL_HEADER_PATH ${CMAKE_SOURCE_DIR}/pub/include)
set(INSTALL_LIB_PATH ${CMAKE_SOURCE_DIR}/pub/lib)
if(WIN32)
ADD_DEFINITIONS()
else()
ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -DSERVER)

endif()

ADD_LIBRARY(security STATIC ${SRC_LIST})
target_include_directories(
        security PRIVATE
        .
        ../pkg/include/security
        /usr/local/include
)
# install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/pkg/include/security
        DESTINATION ${INSTALL_HEADER_PATH}
        FILES_MATCHING PATTERN "*.h")
#install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${CMAKE_PROJECT_NAME})
install(TARGETS security DESTINATION ${INSTALL_LIB_PATH} EXPORT libsecurity)
