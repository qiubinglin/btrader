# 控制逻辑模块构建配置

# 控制逻辑源文件
set(CONTROL_SOURCES
    main.cpp
    application.cpp
    datamanager.cpp
    uimanager.cpp
)

# 控制逻辑头文件
set(CONTROL_HEADERS
    application.h
    datamanager.h
    uimanager.h
)

# 创建控制逻辑库
add_library(tradeview_control STATIC
    application.cpp
    datamanager.cpp
    uimanager.cpp
    application.h
    datamanager.h
    uimanager.h
)

# 创建主应用程序
add_executable(tradeview_app
    main.cpp
    ../qml/qml.qrc
)

# 设置库的属性
set_target_properties(tradeview_control PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 QuickLayouts)

# 链接Qt6库和模型库
target_link_libraries(tradeview_control
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickLayouts
    tradeview_models
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

# 设置包含目录
target_include_directories(tradeview_control PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/model
)

# 安装规则
install(TARGETS tradeview_control tradeview_app
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(FILES ${CONTROL_HEADERS}
    DESTINATION include/tradeview/control
) 