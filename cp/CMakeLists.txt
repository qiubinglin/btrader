file(GLOB src
    *.cpp
)
add_library(computation
    ${src}
)
target_include_directories(computation PUBLIC
    ${PROJECT_SOURCE_DIR}/strategy
)
target_link_libraries(computation PUBLIC strategy core)