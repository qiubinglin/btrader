set(PYJOUR_SRCS
    py_journal.cpp
)

pybind11_add_module(pybtrader
    pybtrader.cpp
    ${PYJOUR_SRCS}
)
target_link_libraries(pybtrader PRIVATE pyservice)