# 订单流交易系统UI模型源文件列表
set(MODEL_SOURCES
    candlestickmodel.cpp      # K线数据模型
    ticktrademodel.cpp        # 逐笔成交明细模型
    orderbookmodel.cpp        # 买卖档位模型
    footprintmodel.cpp        # 足迹图模型
    microorderbookmodel.cpp   # 微盘口模型
)

# 订单流交易系统UI模型头文件列表
set(MODEL_HEADERS
    candlestickmodel.h        # K线数据模型头文件
    ticktrademodel.h          # 逐笔成交明细模型头文件
    orderbookmodel.h          # 买卖档位模型头文件
    footprintmodel.h          # 足迹图模型头文件
    microorderbookmodel.h     # 微盘口模型头文件
)

# 创建订单流交易系统UI模型静态库
add_library(tradeview_models STATIC
    ${MODEL_SOURCES}
    ${MODEL_HEADERS}
)

# 设置库的属性
set_target_properties(tradeview_models PROPERTIES
    AUTOMOC ON      # 启用Qt的元对象编译器
    AUTOUIC OFF     # 禁用UI编译器（纯C++模型）
    AUTORCC OFF     # 禁用资源编译器
)

# 检查Qt6是否找到
if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found. Please check your Qt6 installation at ${CMAKE_PREFIX_PATH}")
endif()

# 链接Qt6库
target_link_libraries(tradeview_models
    Qt6::Core
)

# 设置包含目录
target_include_directories(tradeview_models PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
