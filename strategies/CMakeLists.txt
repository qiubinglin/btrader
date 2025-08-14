include_directories(${PROJECT_SOURCE_DIR}/strategy)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/strategylib)
    add_subdirectory(strategylib)
endif()

add_library(strategies SHARED
    my_strategy.cpp
)
target_link_libraries(strategies PUBLIC strategy)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/strategylib)
    target_link_libraries(strategies PUBLIC strategylib)
endif()