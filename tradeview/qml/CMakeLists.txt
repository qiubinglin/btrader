# QML资源库构建配置

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 QuickLayouts)

# 设置QML资源文件
set(QML_RESOURCES
    qml.qrc
)

# 创建QML资源库
add_library(tradeview_qml_resources STATIC
    main.qml
    ${QML_RESOURCES}
)

# 设置库的属性
set_target_properties(tradeview_qml_resources PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC ON
)

# 链接Qt6库
target_link_libraries(tradeview_qml_resources
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickLayouts
)

# 安装规则
install(TARGETS tradeview_qml_resources
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装QML文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION share/tradeview/qml
    FILES_MATCHING PATTERN "*.qml"
    PATTERN "*.qrc"
    PATTERN "*.md"
) 