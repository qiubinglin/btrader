# Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Quick Qml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    main.cpp
    model/order.cpp
    control/order_manager.cpp
)

qt5_add_resources(RESOURCE_FILES
    resources.qrc
)

add_executable(tradeviewer
    ${SOURCES}
    ${RESOURCE_FILES}
)
target_link_libraries(tradeviewer PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Qml
)
set_target_properties(tradeviewer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_include_directories(tradeviewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)