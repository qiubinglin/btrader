file(GLOB src "*.cpp")

add_library(extension STATIC ${src})

target_link_libraries(extension PRIVATE core)