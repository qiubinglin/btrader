file(GLOB src
    td_engine.cpp
)
add_library(td
    ${src}
)
target_include_directories(td PUBLIC
    ${PROJECT_SOURCE_DIR}/broker
)
target_link_libraries(td broker core)