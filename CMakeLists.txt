cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0077 NEW)
project(btrader)

include(cmake/compile_standard.cmake)
include(cmake/thirdparty.cmake)
include(cmake/compile_option.cmake)
include(cmake/macro.cmake)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(infra)
add_subdirectory(core)
add_subdirectory(broker)
add_subdirectory(engines)
add_subdirectory(strategy)
add_subdirectory(strategies)
add_subdirectory(algorithm)

add_subdirectory(pylib)

add_subdirectory(main)
add_subdirectory(tests)
add_subdirectory(tradeview)
add_subdirectory(extension)

file(COPY ${CMAKE_SOURCE_DIR}/config-tpl DESTINATION ${CMAKE_BINARY_DIR})