include(ExternalProject)

set(CURL_NAME curl-7.82.0)
set(CURL_ROOT            ${CMAKE_CURRENT_BINARY_DIR}/${CURL_NAME})
set(CURL_SOURCE_PATH      ${CURL_ROOT}/src/${CURL_NAME})

set(CURL_URL             ${CMAKE_SOURCE_DIR}/module/local/${CURL_NAME}.tar.gz)
set(CURL_CONFIGURE      ${CURL_SOURCE_PATH}/configure --prefix=/usr/local/ --with-openssl=/usr/local)
set(CURL_MAKE            make -j 16)
set(CURL_INSTALL         make install)

ExternalProject_Add(${CURL_NAME}
        URL                  ${CURL_URL}
        PREFIX               ${CURL_ROOT}
        CONFIGURE_COMMAND    ${CURL_CONFIGURE}
        BUILD_COMMAND        ${CURL_MAKE}
        INSTALL_COMMAND      ${CURL_INSTALL}
        )