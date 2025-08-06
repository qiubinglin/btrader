# 控制逻辑模块构建配置

file(GLOB CONTROL_SOURCES "*.cpp")
file(GLOB CONTROL_HEADERS "*.h")

# 创建控制逻辑库
add_library(tradeview_control STATIC
    ${CONTROL_SOURCES}
    ${CONTROL_HEADERS}
)

# 设置库的属性
set_target_properties(tradeview_control PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

# 链接Qt6库和模型库
target_link_libraries(tradeview_control
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickLayouts
    tradeview_models
    core
    guidb
)

target_include_directories(tradeview_control PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)