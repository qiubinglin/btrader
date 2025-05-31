option(HP "Enable high-frequency" OFF)
if(HP)
    add_definitions(-DHP)
endif()