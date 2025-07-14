file(GLOB src
    *.cpp
    journal/*.cpp
)
add_library(core
    ${src}
)
target_link_libraries(core PUBLIC
    infra
    fmt::fmt    
)
target_include_directories(core PUBLIC
    ${PROJECT_SOURCE_DIR}/thirdparty/RxCpp
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/core/journal
)
target_include_directories(core PRIVATE
    ${PROJECT_SOURCE_DIR}
)

configure_file(main_cfg.tpl.json ${PROJECT_BINARY_DIR}/main_cfg.tpl.json COPYONLY)