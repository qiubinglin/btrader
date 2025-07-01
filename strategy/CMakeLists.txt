file(GLOB src
    *.cpp
)
add_library(strategy
    ${src}
)
target_link_libraries(strategy PUBLIC core)