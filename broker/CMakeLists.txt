add_subdirectory(binance)
add_subdirectory(backtest)

file(GLOB src
    *.cpp
)
add_library(broker
    ${src}
)

set(XTP_LIBS
    /root/libs/libxtpquoteapi.so
    /root/libs/libxtptraderapi.so
)

target_link_libraries(broker
    core
    binance
    backtest_service
    # ${XTP_LIBS}
)
target_include_directories(broker PUBLIC
    ${PROJECT_SOURCE_DIR}/core
    # ${PROJECT_SOURCE_DIR}/thirdparty/XTP_API/include
)