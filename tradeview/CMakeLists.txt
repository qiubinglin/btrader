# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 QuickLayouts)

add_subdirectory(model)
add_subdirectory(qml)
add_subdirectory(control)

# 创建主应用程序
add_executable(tradeview_app
    main.cpp
    qml/qml.qrc
)

# 设置主应用程序属性
set_target_properties(tradeview_app PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC ON
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# 链接主应用程序
target_link_libraries(tradeview_app
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickLayouts
    tradeview_models
    tradeview_control
    tradeview_qml_resources
)

file(COPY ${CMAKE_SOURCE_DIR}/tradeview/run_tradeview.sh DESTINATION ${CMAKE_BINARY_DIR})
