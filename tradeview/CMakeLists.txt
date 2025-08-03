add_subdirectory(model)
add_subdirectory(qml)
add_subdirectory(control)

file(COPY ${CMAKE_SOURCE_DIR}/tradeview/run_tradeview.sh DESTINATION ${CMAKE_BINARY_DIR})
