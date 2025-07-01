file(GLOB src
    *.cpp
)
add_library(pyservice ${src})
target_link_libraries(pyservice PUBLIC core infra pybind11::embed)