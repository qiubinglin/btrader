include_directories(${PROJECT_SOURCE_DIR}/strategy)

add_library(MyStrategy SHARED
    my_strategy.cpp
)
target_link_libraries(MyStrategy strategy)