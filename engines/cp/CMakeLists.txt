file(GLOB src
    *.cpp
)
add_library(computation
    ${src}
)
target_include_directories(computation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/strategy
)
target_link_libraries(computation PUBLIC strategy core extension)