include(FetchContent)

# FetchContent_Declare(
#   uWebSockets
#   GIT_REPOSITORY https://github.com/uNetworking/uWebSockets
#   GIT_TAG        b9b59b2b164489f3788223fec5821f77f7962d43) # v20.71.0
# FetchContent_MakeAvailable(uWebSockets)

find_package(fmt)

# OpenSSL
find_package(OpenSSL REQUIRED)
# include_directories(${OPENSSL_INCLUDE_DIR})

# spdlog
find_package(spdlog REQUIRED)

# pybind11
# If can't find pybind11 then do: export pybind11_DIR=/path/to/pybind11/share/cmake/pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# ta-lib
set(TA-LIB ta-lib-static)

add_subdirectory(thirdparty)