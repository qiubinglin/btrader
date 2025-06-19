file(GLOB src
    *.cpp
)
add_library(backtest_service ${src})
target_link_libraries(backtest_service core infra)