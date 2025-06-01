file(GLOB src
    *.cpp
    detail/*.cpp
    web/*.cpp
    epoll_usage.c
)
set(crypto_src
    crypto/signer.cpp
    crypto/ed25519.c
    crypto/base64.c
)
add_library(infra
    ${src}
    ${crypto_src}
)
target_include_directories(infra PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/nng/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/libhv/include
    ${OPENSSL_INCLUDE_DIR} # openssl
)
target_include_directories(infra PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/detail
    ${CMAKE_CURRENT_SOURCE_DIR}/web
)
target_link_libraries(infra
    nng
    fmt::fmt
    hv_static
    ${OPENSSL_LIBRARIES} # openssl
    spdlog::spdlog # spdlog
)

add_executable(MemIPCTest
    ./tests/MemIPCTest.cpp
)
target_link_libraries(MemIPCTest infra)