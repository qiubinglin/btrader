include_directories(${PROJECT_SOURCE_DIR}/strategy)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/strategylib)
    add_subdirectory(strategylib)
endif()

file(GLOB src
    *.cpp
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/strategylib)
    add_library(strategies SHARED
        ${src}
        $<TARGET_OBJECTS:strategylib>
    )
else()
    add_library(strategies SHARED
        ${src}
    )
endif()
target_link_libraries(strategies PUBLIC strategy)
