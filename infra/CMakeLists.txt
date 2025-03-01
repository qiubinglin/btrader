file(GLOB src
    *.cpp
    detail/*.cpp
)
add_library(infra
    ${src}
)
target_include_directories(infra PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/nng/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/libhv/include
)
target_include_directories(infra PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/detail
)
target_link_libraries(infra
    nng
    fmt::fmt
    hv_static
)

add_executable(MemIPCTest
    ./tests/MemIPCTest.cpp
)
target_link_libraries(MemIPCTest infra)