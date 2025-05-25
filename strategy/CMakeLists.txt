file(GLOB src
    executor.cpp
)
add_library(strategy
    ${src}
)
target_link_libraries(strategy core)