cmake_minimum_required(VERSION 3.29)
project(dhost)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 26)

set(COREHOST_BASE https://raw.githubusercontent.com/dotnet/runtime/main/src/native/corehost)
file(DOWNLOAD ${COREHOST_BASE}/coreclr_delegates.h ${CMAKE_CACHEFILE_DIR}/dep/coreclr_delegates.h)
file(DOWNLOAD ${COREHOST_BASE}/hostfxr.h ${CMAKE_CACHEFILE_DIR}/dep/hostfxr.h)
file(DOWNLOAD ${COREHOST_BASE}/nethost/nethost.h ${CMAKE_CACHEFILE_DIR}/dep/nethost.h)
include_directories(${CMAKE_CACHEFILE_DIR}/dep)

add_library(dhost SHARED library.cpp)
target_compile_options(dhost PRIVATE -O3 -fno-rtti)

add_executable(dhost-test main.cpp)
target_link_directories(dhost-test PRIVATE "/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.arch-x64/8.0.3/runtimes/arch-x64/native/")
target_link_libraries(dhost-test PRIVATE dhost nethost)