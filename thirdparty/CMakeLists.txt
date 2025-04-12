remove_definitions(-Werror)
add_subdirectory(nng)

set(WITH_OPENSSL ON) # For libhv compile
add_subdirectory(libhv)
unset(WITH_OPENSSL)
add_definitions(-Werror)