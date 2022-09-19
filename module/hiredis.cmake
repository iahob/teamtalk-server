# 添加第三方依赖包
include(ExternalProject)
set(HIREDIS_NAME hiredis-1.0.2)
set(HIREDIS_ROOT          ${CMAKE_CURRENT_BINARY_DIR}/${HIREDIS_NAME})
set(HIREDIS_SOURCE_PATH ${HIREDIS_ROOT}/src/${HIREDIS_NAME})

ExternalProject_Add(${HIREDIS_NAME}
        PREFIX ${HIREDIS_ROOT}
        #--Download step--------------
        URL ${CMAKE_SOURCE_DIR}/module/local/hiredis-1.0.2.tar.gz
        #URL_HASH SHA1=7fdb90a2d45085feb8b76167cae419ad4c211d6b
        #--Configure step-------------
        CONFIGURE_COMMAND ${CMAKE_COMMAND} ${HIREDIS_SOURCE_PATH}
        -DCMAKE_INSTALL_PREFIX=/usr/local/
        #--Build step-----------------
        BUILD_COMMAND   make -j 8
        #--Install step---------------
      #  UPDATE_COMMAND "" # Skip annoying updates for every build
        INSTALL_COMMAND make install
        )

