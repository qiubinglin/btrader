set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) 

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    # set(CMAKE_BUILD_TYPE RelWithDebInfo)  # -g -O2
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC") # set -fPIC for nng
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_compile_options(
    -Wall        # 打开大部分警告
    -Wextra      # 打开额外的警告
    -Wpedantic   # 遵循语言标准
)