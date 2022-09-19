include(ExternalProject)

set(YAML-CPP_ROOT          ${CMAKE_CURRENT_BINARY_DIR}/local/yaml-cpp-0.6.3)

ExternalProject_Add(yaml-cpp-0.6.3
        PREFIX ${YAML-CPP_ROOT}
        #--Download step--------------
        URL ${CMAKE_SOURCE_DIR}/thirdpart/yaml-cpp-0.6.3.tar.gz
        #URL_HASH SHA1=7fdb90a2d45085feb8b76167cae419ad4c211d6b
        #--Configure step-------------
        CONFIGURE_COMMAND cd ${YAML-CPP_ROOT}/src/yaml-cpp-0.6.3  &&  cmake -D CMAKE_INSTALL_PREFIX=${YAML-CPP_ROOT} .
        #--Build step-----------------
        BUILD_COMMAND cd ${YAML-CPP_ROOT}/src/yaml-cpp-0.6.3 && make
        #--Install step---------------
        UPDATE_COMMAND "" # Skip annoying updates for every build
        INSTALL_COMMAND cd ${YAML-CPP_ROOT}/src/yaml-cpp-0.6.3 && make install
        )

SET(YAML_CPP_INCLUDE_DIR ${YAML-CPP_ROOT}/include)
SET(YAML_CPP_LIB_DIR  ${YAML-CPP_ROOT}/lib)
#
#add_library(yaml-cpp SHARED   IMPORTED)
#set_target_properties(yaml-cpp PROPERTIES IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/libyaml-cpp.so)
#
