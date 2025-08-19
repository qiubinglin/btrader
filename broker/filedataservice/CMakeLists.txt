file(GLOB src
    *.cpp
)
add_library(filedataservice ${src})
target_link_libraries(filedataservice PUBLIC core infra)