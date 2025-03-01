cmake_minimum_required(VERSION 3.15)
project(btrader)

include(cmake/thirdparty.cmake)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC") # set -fPIC for nng
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_BUILD_TYPE "Debug")
message("Build type: ${CMAKE_BUILD_TYPE}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(infra)
add_subdirectory(core)
add_subdirectory(broker)
add_subdirectory(md)
add_subdirectory(td)
add_subdirectory(cp)
add_subdirectory(strategy)
add_subdirectory(strategies)

add_subdirectory(main)