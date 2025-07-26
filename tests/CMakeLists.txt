add_executable(binance_test binance_test.cpp)
target_link_libraries(binance_test broker infra OpenSSL::SSL OpenSSL::Crypto)

# Test websocket
add_executable(websocket_server_test websocket_server_test.cpp)
target_link_libraries(websocket_server_test infra)
add_executable(websocket_client_test websocket_client_test.cpp)
target_link_libraries(websocket_client_test infra)

# Test http
add_executable(http_test http_test.cpp)
target_link_libraries(http_test infra)

add_executable(ed25519_test ed25519_test.c)
target_link_libraries(ed25519_test infra)

add_executable(crypto_signer_test crypto_signer_test.cpp)
target_link_libraries(crypto_signer_test infra)

add_executable(journal_epoll_test journal_epoll_test.cpp)
target_link_libraries(journal_epoll_test core)

# spd test
add_executable(spdlog_demo spdlog_demo.cpp)
target_link_libraries(spdlog_demo spdlog::spdlog)

# pybind11 test
pybind11_add_module(pybind11_test pybind11_test.cpp)

# time test
add_executable(time_test time_test.cpp)
target_link_libraries(time_test infra)

# Test for journal communication
add_executable(journal_comm_test journal_comm_test.cpp)
target_link_libraries(journal_comm_test pyinterface)