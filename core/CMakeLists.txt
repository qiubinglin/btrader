file(GLOB src
    *.cpp
    journal/*.cpp
)
add_library(core
    ${src}
)
target_link_libraries(core PUBLIC
    infra
    extension
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
