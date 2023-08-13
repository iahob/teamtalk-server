cmake_minimum_required(VERSION 3.12)
project(Base)

# 设置源文件列表
file(GLOB_RECURSE SRC_LIST
        "*.cpp"
        "../pb/protocol/*.cpp"
)

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
add_library(base_shared SHARED ${SRC_LIST})
# 生成静态库
add_library(base_static STATIC ${SRC_LIST})

target_include_directories(base_static PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../pb/protocol
        ${CMAKE_CURRENT_SOURCE_DIR}/include/base
        /usr/local/include
)
target_include_directories(base_shared PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../pb/protocol
        ${CMAKE_CURRENT_SOURCE_DIR}/include/base
        /usr/local/include
)
target_link_libraries(base_static PRIVATE pthread crypto ssl protobuf jsoncpp spdlog)
target_link_libraries(base_shared PRIVATE pthread crypto ssl protobuf jsoncpp spdlog)

target_link_directories(base_static PRIVATE /usr/local/lib)
target_link_directories(base_shared PRIVATE /usr/local/lib)

set_target_properties(base_static PROPERTIES OUTPUT_NAME base)
set_target_properties(base_shared PROPERTIES OUTPUT_NAME base)
# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${INSTALL_HEADER_PATH})

# 安装库文件
install(TARGETS base_static base_shared
        DESTINATION  ${INSTALL_LIB_PATH}
)