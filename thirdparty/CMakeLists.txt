set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC") # set -fPIC for nng
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

remove_definitions(-Werror)
set(NNG_TESTS OFF)
add_subdirectory(nng)
unset(NNG_TESTS)

set(WITH_OPENSSL ON) # For libhv compile
add_subdirectory(libhv)
unset(WITH_OPENSSL)
add_definitions(-Werror)

add_subdirectory(MurmurHash3)
add_subdirectory(ta-lib)