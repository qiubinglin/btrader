

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC") # set -fPIC for nng
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_compile_options(
    -Wall        # 打开大部分警告
    -Wextra      # 打开额外的警告
    -Wpedantic   # 遵循语言标准
    -Werror      # 将所有警告视为错误
    -Wno-unused-parameter  # 忽略未使用的参数警告
)