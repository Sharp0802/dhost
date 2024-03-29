cmake_minimum_required(VERSION 3.29)
project(dhost LANGUAGES CXX)

set(NETHOST_RUNTIME_IDENTIFIER linux-x64)
set(NETHOST_RUNTIME_VERSION "8.0.3")

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 26)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/out/)

function(PrepareDotNetLibraries)
    set(NUGET_BASE "https://www.nuget.org/api/v2/package")
    set(NETHOST_RUNTIME_ARCHIVE "coreclr.${NETHOST_RUNTIME_IDENTIFIER}.${NETHOST_RUNTIME_VERSION}.zip")

    file(
        MAKE_DIRECTORY
        "${CMAKE_BINARY_DIR}/dep/"
    )
    file(
        DOWNLOAD
        "${NUGET_BASE}/runtime.${NETHOST_RUNTIME_IDENTIFIER}.Microsoft.NETCore.DotNetAppHost/${NETHOST_RUNTIME_VERSION}"
        "${CMAKE_BINARY_DIR}/dep/${NETHOST_RUNTIME_ARCHIVE}"
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/dep/${NETHOST_RUNTIME_ARCHIVE}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/dep/"
    )

    set(NETHOST_RUNTIME_DIR "${CMAKE_BINARY_DIR}/dep/runtimes/${NETHOST_RUNTIME_IDENTIFIER}/native")

    include_directories("${NETHOST_RUNTIME_DIR}")
    link_directories("${NETHOST_RUNTIME_DIR}")

    if (NETHOST_RUNTIME_IDENTIFIER MATCHES "win$")
        set(NETHOST_LIB "${NETHOST_RUNTIME_DIR}/nethost.dll")
    elseif (NETHOST_RUNTIME_IDENTIFIER MATCHES "osx$")
        set(NETHOST_LIB "${NETHOST_RUNTIME_DIR}/libnethost.dylib")
    else ()
        set(NETHOST_LIB "${NETHOST_RUNTIME_DIR}/libnethost.so")
    endif ()
    file(GLOB NETHOST_INC "${NETHOST_RUNTIME_DIR}/*.h")
    file(COPY ${NETHOST_LIB} DESTINATION ${OUTPUT_DIR}/bin)
    file(COPY ${NETHOST_INC} DESTINATION ${OUTPUT_DIR}/inc)
endfunction()

PrepareDotNetLibraries()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/dhost.h DESTINATION ${OUTPUT_DIR}/inc)

add_library(dhost SHARED library.cpp)
target_link_libraries(dhost PRIVATE nethost)
target_compile_options(dhost PRIVATE -O3 -fno-rtti)
set_target_properties(dhost PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin
)

add_executable(dhost-test main.cpp)
target_link_libraries(dhost-test PRIVATE dhost nethost)