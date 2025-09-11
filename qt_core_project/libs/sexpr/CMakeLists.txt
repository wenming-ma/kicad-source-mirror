# Sexpr library for minimum PCB set

add_library( sexpr STATIC
    sexpr.cpp
    sexpr_parser.cpp
)

target_include_directories( sexpr PUBLIC
    include
    ${wxWidgets_INCLUDE_DIRS}
)

target_compile_definitions( sexpr PUBLIC
    ${wxWidgets_DEFINITIONS}
)

target_link_libraries( sexpr
    ${wxWidgets_LIBRARIES}
)