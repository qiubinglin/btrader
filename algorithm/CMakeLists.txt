file(GLOB src
    *.cpp
)
add_library(algorithm ${src})
target_include_directories(algorithm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)