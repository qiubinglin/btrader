file(GLOB src
    *.cpp
)
add_library(brokersim ${src})
target_link_libraries(brokersim PUBLIC core infra)