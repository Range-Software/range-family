qt_add_executable(family-tool
    src/application.cpp
    src/main.cpp
    src/main_task.cpp
    src/application.h
    src/main_task.h
)

add_dependencies(family-tool range-base-lib range-family-lib)

target_link_libraries(family-tool
    PRIVATE
        Qt6::Core
        common_defines
        range-base-lib
        range-family-lib
)
