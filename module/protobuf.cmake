include(ExternalProject)

set(PROTOBUF_NAME protobuf-all-3.18.1)
set(PROTOBUF_ROOT            ${CMAKE_CURRENT_BINARY_DIR}/${PROTOBUF_NAME})
set(HIREDIS_SOURCE_PATH      ${PROTOBUF_ROOT}/src/${PROTOBUF_NAME})

set(PROTOBUF_URL             ${CMAKE_SOURCE_DIR}/module/local/${PROTOBUF_NAME}.tar.gz)
set(PROTOBUF_CONFIGURE       ${HIREDIS_SOURCE_PATH}/configure --prefix=/usr/local/)
set(PROTOBUF_MAKE            make -j 8)
set(PROTOBUF_INSTALL         make install)

ExternalProject_Add(${PROTOBUF_NAME}
        URL                  ${PROTOBUF_URL}
        PREFIX               ${PROTOBUF_ROOT}
        CONFIGURE_COMMAND    ${PROTOBUF_CONFIGURE}
        BUILD_COMMAND        ${PROTOBUF_MAKE}
        INSTALL_COMMAND      ${PROTOBUF_INSTALL}
)





