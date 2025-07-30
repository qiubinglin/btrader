file(GLOB src
    md_engine.cpp
)
add_library(md
    ${src}
)
target_include_directories(md PUBLIC
    ${PROJECT_SOURCE_DIR}/broker
)
target_link_libraries(md PUBLIC broker core)