remove_definitions(-Werror)
add_subdirectory(nng)
set(WITH_OPENSSL ON) # For libhv compile
add_subdirectory(libhv)
add_definitions(-Werror)