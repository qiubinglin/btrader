cmake_minimum_required(VERSION 3.15)
project(btrader)

include(cmake/compile.cmake)
include(cmake/thirdparty.cmake)
include(cmake/macro.cmake)

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
add_subdirectory(algorithm)

add_subdirectory(main)
add_subdirectory(tests)