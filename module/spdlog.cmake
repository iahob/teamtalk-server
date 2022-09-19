include(ExternalProject)

set(SPDLOG_NAME spdlog-1.8.5)
set(SPDLOG_ROOT            ${CMAKE_CURRENT_BINARY_DIR}/${SPDLOG_NAME})
set(SPDLOG_SOURCE_PATH      ${SPDLOG_ROOT}/src/${SPDLOG_NAME})

set(SPDLOG_URL             ${CMAKE_SOURCE_DIR}/module/local/${SPDLOG_NAME}.tar.gz)
set(SPDLOG_CONFIGURE        ${CMAKE_COMMAND} ${SPDLOG_SOURCE_PATH} -DCMAKE_INSTALL_PREFIX=/usr/local/)
set(SPDLOG_MAKE            make -j 8)
set(SPDLOG_INSTALL         make install)

ExternalProject_Add(
        ${SPDLOG_NAME}
        URL                  ${SPDLOG_URL}
        PREFIX               ${SPDLOG_ROOT}
        CONFIGURE_COMMAND    ${SPDLOG_CONFIGURE}
        BUILD_COMMAND        ${SPDLOG_MAKE}
        INSTALL_COMMAND      ${SPDLOG_INSTALL}
        )





