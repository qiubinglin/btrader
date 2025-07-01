pybind11_add_module(pybtrader
    pybtrader.cpp
)
target_link_libraries(pybtrader PRIVATE pyservice)