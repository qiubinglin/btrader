file(GLOB src
    main.cpp
    mentor.cpp
    mentor_run.cpp
    option_parser.cpp
)
add_executable(btrader
    ${src}
)
target_include_directories(btrader PRIVATE
    ${PROJECT_SOURCE_DIR}/cp
    ${PROJECT_SOURCE_DIR}/md
    ${PROJECT_SOURCE_DIR}/td
)
target_link_libraries(btrader PUBLIC computation md td infra)

configure_file(main.sh ${PROJECT_BINARY_DIR}/main.sh COPYONLY)