file(GLOB src
    *.cpp
)
add_library(binance ${src})
target_link_libraries(binance PUBLIC core infra)