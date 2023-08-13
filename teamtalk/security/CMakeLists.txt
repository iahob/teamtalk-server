cmake_minimum_required(VERSION 3.10)
project(security)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(. SRC_LIST)
# 设置输出路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(INSTALL_HEADER_PATH ${CMAKE_SOURCE_DIR}/public)
set(INSTALL_LIB_PATH ${CMAKE_SOURCE_DIR}/public/lib)
# 设置编译选项
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
        -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
        -DAC_HAS_DEBUG -DLINUX_DAEMON)
# 生成动态库
add_library(security_shared SHARED ${SRC_LIST})
# 生成静态库
add_library(security_static STATIC ${SRC_LIST})

target_include_directories(security_shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/local/include)
target_include_directories(security_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/local/include)

set_target_properties(security_shared PROPERTIES OUTPUT_NAME security)
set_target_properties(security_static PROPERTIES OUTPUT_NAME security)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${INSTALL_HEADER_PATH})
# 安装库文件
install(TARGETS security_shared security_static DESTINATION  ${INSTALL_LIB_PATH}
)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/security DESTINATION ${INSTALL_HEADER_PATH} FILES_MATCHING PATTERN "*.h")
#install(TARGETS security DESTINATION ${INSTALL_LIB_PATH} EXPORT libsecurity)
