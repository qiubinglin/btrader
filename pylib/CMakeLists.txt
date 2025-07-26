set(PYJOUR_SRCS
    py_journal.cpp
)
add_library(pyinterface
    ${PYJOUR_SRCS}
)
target_link_libraries(pyinterface PUBLIC
    pybind11::embed
    pyservice
)

pybind11_add_module(pybtrader
    pybtrader.cpp
)
target_link_libraries(pybtrader PRIVATE
    pyinterface
)