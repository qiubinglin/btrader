file(GLOB src
    *.cpp
)
add_library(guidb ${src})
target_link_libraries(guidb
    Qt6::Core
    core
)
target_include_directories(guidb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)