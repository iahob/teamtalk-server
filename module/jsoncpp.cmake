include(ExternalProject)

set(JSONCPP_NAME jsoncpp-1.9.5)
set(JSONCPP_ROOT            ${CMAKE_CURRENT_BINARY_DIR}/${JSONCPP_NAME})
set(JSONCPP_SOURCE_PATH      ${JSONCPP_ROOT}/src/${JSONCPP_NAME})

set(JSONCPP_URL             ${CMAKE_SOURCE_DIR}/module/local/${JSONCPP_NAME}.tar.gz)
set(JSONCPP_CONFIGURE        ${CMAKE_COMMAND} ${JSONCPP_SOURCE_PATH} -DCMAKE_INSTALL_PREFIX=/usr/local/)
set(JSONCPP_MAKE            pwd > a.txt && make -j 16)
set(JSONCPP_INSTALL         make install)

ExternalProject_Add(
        ${JSONCPP_NAME}
        URL                  ${JSONCPP_URL}
        PREFIX               ${JSONCPP_ROOT}
        CONFIGURE_COMMAND    ${JSONCPP_CONFIGURE}
        BUILD_COMMAND        ${JSONCPP_MAKE}
        INSTALL_COMMAND      ${JSONCPP_INSTALL}
)





