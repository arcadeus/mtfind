cmake_minimum_required(VERSION 3.5)

#
#---- Build ----
#
project(mtfind LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mtfind main.cpp Range.hpp)

#
#---- Prepare Test ----
#
set(
    MTFIND_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/test.sh"
)
add_custom_target(
    mtfind.script ALL
    COMMAND
        chmod a+x ${MTFIND_SCRIPT}
    SOURCES
        ${MTFIND_SCRIPT}
)

set(
    MTFIND_STATIC
    ${CMAKE_CURRENT_LIST_DIR}/input.txt
    ${CMAKE_CURRENT_LIST_DIR}/test-output.txt
)
add_custom_target(
    mtfind.static ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        ${MTFIND_STATIC}
        ${CMAKE_BINARY_DIR}
    SOURCES
        ${MTFIND_STATIC}
)

#
#---- Test ----
#
include(CTest)

add_test(
    NAME mtfind.test
    COMMAND ${MTFIND_SCRIPT}
)
