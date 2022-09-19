include(ExternalProject)

set(OPENSSL_NAME openssl-openssl-3.0.2)
set(OPENSSL_ROOT            ${CMAKE_CURRENT_BINARY_DIR}/${OPENSSL_NAME})
set(OPENSSL_SOURCE_PATH      ${OPENSSL_ROOT}/src/${OPENSSL_NAME})

set(OPENSSL_URL             ${CMAKE_SOURCE_DIR}/module/local/${OPENSSL_NAME}.tar.gz)
set(OPENSSL_CONFIGURE       ${OPENSSL_SOURCE_PATH}/Configure --prefix=/usr/local/)
set(OPENSSL_MAKE            make -j 8)
set(OPENSSL_INSTALL         make install)

ExternalProject_Add(${OPENSSL_NAME}
        URL                  ${OPENSSL_URL}
        PREFIX               ${OPENSSL_ROOT}
        CONFIGURE_COMMAND    ${OPENSSL_CONFIGURE}
        BUILD_COMMAND        ${OPENSSL_MAKE}
        INSTALL_COMMAND      ${OPENSSL_INSTALL}
        )





