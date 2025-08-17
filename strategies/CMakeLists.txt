include_directories(${PROJECT_SOURCE_DIR}/strategy)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/strategylib)
    add_subdirectory(strategylib)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/strategylib)
    add_library(strategies SHARED
        my_strategy.cpp
        $<TARGET_OBJECTS:strategylib>
    )
else()
    add_library(strategies SHARED
        my_strategy.cpp
    )
endif()
target_link_libraries(strategies PUBLIC strategy)
