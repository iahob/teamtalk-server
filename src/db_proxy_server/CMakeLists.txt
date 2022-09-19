cmake_minimum_required(VERSION 3.10)
PROJECT(db_proxy_server)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
AUX_SOURCE_DIRECTORY(./business SRC_LIST)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/pub/bin)





#设置自己mac os的路径
if(APPLE)
    SET(MYSQL_INCLUDE_DIR /usr/local/Cellar/mysql/8.0.28_1/include)
    SET(MYSQL_LIB /usr/local/Cellar/mysql/8.0.28_1/lib)
else()
    SET(MYSQL_INCLUDE_DIR /usr/include/mysql)
endif()
    

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11 -DENCRYPT)

set(CMAKE_CXX_STANDARD 11)


ADD_EXECUTABLE(db_proxy_server ${SRC_LIST})

TARGET_LINK_LIBRARIES(db_proxy_server pthread base protobuf mysqlclient hiredis curl ssl crypto jsoncpp spdlog)
 
TARGET_INCLUDE_DIRECTORIES(db_proxy_server PRIVATE
        ./
        ./businiess
        ../pkg/include
         ../pb/protocol
         /usr/local/include/
         ${MYSQL_INCLUDE_DIR}


)

TARGET_LINK_DIRECTORIES( db_proxy_server PRIVATE
        ./
        /usr/local/lib
        ../pkg/lib
        ${MYSQL_LIB}
)