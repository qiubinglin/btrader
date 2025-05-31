set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) 
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC") # set -fPIC for nng
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_BUILD_TYPE "Debug")
message("Build type: ${CMAKE_BUILD_TYPE}")

add_compile_options(
    -Wall        # 打开大部分警告
    -Wextra      # 打开额外的警告
    -Wpedantic   # 遵循语言标准
)