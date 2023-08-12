cmake_minimum_required(VERSION 3.15)

include(ExternalProject)

set(YAML-CPP_VERSION "yaml-cpp-0.8.0")
set(YAML-CPP_ROOT ${CMAKE_CURRENT_BINARY_DIR}/${YAML-CPP_VERSION})
#set(YAML-CPP_SOURCE_PATH "${YAML-CPP_ROOT}/teamtalk/${YAML-CPP_VERSION}")
set(YAML-CPP_URL ${CMAKE_SOURCE_DIR}/module/local/${YAML-CPP_VERSION}.tar.gz)
set(YAML-CPP_CONFIGURE ${CMAKE_COMMAND} ../${YAML-CPP_VERSION} -DCMAKE_INSTALL_PREFIX=/usr/local/)
set(YAML-CPP_MAKE_COMMAND make -j 16)
set(YAML-CPP_INSTALL_COMMAND make install)



ExternalProject_Add(
        ${YAML-CPP_VERSION}
        URL ${YAML-CPP_URL}
        PREFIX ${YAML-CPP_ROOT}
        CONFIGURE_COMMAND ${YAML-CPP_CONFIGURE}
        BUILD_COMMAND ${YAML-CPP_MAKE_COMMAND}
        INSTALL_COMMAND ${YAML-CPP_INSTALL_COMMAND}
)


#cmake_minimum_required(VERSION 3.15)
#
#include(ExternalProject)
#
#set(YAML-CPP_VERSION "yaml-cpp-0.6.3")
#set(YAML-CPP_ROOT ${CMAKE_CURRENT_BINARY_DIR}/${YAML-CPP_VERSION})
#set(YAML-CPP_SOURCE_PATH "${YAML-CPP_ROOT}/teamtalk/${YAML-CPP_VERSION}")
#set(YAML-CPP_URL ${CMAKE_SOURCE_DIR}/module/local/${YAML-CPP_VERSION}.tar.gz)
#set(YAML-CPP_CONFIGURE_COMMAND ${CMAKE_COMMAND} ${YAML-CPP_SOURCE_PATH} -D CMAKE_INSTALL_PREFIX=/usr/local/)
#set(YAML-CPP_MAKE_COMMAND make)
#set(YAML-CPP_INSTALL_COMMAND make install)
#
#ExternalProject_Add(
#        ${YAML-CPP_VERSION}
#        URL ${YAML-CPP_URL}
#        PREFIX ${YAML-CPP_ROOT}
#        CONFIGURE_COMMAND ${YAML-CPP_CONFIGURE_COMMAND}
#        BUILD_COMMAND ${YAML-CPP_MAKE_COMMAND}
#        INSTALL_COMMAND ${YAML-CPP_INSTALL_COMMAND}
#)


#
#
#include(ExternalProject)
#
#set(YAML-CPP_ROOT          ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp-0.6.3)
#
#ExternalProject_Add(yaml-cpp-0.6.3
#        PREFIX ${YAML-CPP_ROOT}
#        #--Download step--------------
#        URL ${CMAKE_SOURCE_DIR}/module/local/yaml-cpp-0.6.3.tar.gz
#        #URL_HASH SHA1=7fdb90a2d45085feb8b76167cae419ad4c211d6b
#        #--Configure step-------------
#        CONFIGURE_COMMAND cd ${YAML-CPP_ROOT}/teamtalk/yaml-cpp-0.6.3  &&  cmake -D CMAKE_INSTALL_PREFIX=${YAML-CPP_ROOT} .
#        #--Build step-----------------
#        BUILD_COMMAND  make
#        #--Install step---------------
#        UPDATE_COMMAND "" # Skip annoying updates for every build
#        INSTALL_COMMAND  make install
#        )
#
#SET(YAML_CPP_INCLUDE_DIR ${YAML-CPP_ROOT}/include)
#SET(YAML_CPP_LIB_DIR  ${YAML-CPP_ROOT}/lib)
##
##add_library(yaml-cpp SHARED   IMPORTED)
##set_target_properties(yaml-cpp PROPERTIES IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/libyaml-cpp.so)
##
