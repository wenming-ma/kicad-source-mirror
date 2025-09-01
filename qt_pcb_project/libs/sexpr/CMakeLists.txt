# Sexpr library for minimum PCB set

add_library( sexpr STATIC
    sexpr.cpp
    sexpr_parser.cpp
)

target_include_directories( sexpr PUBLIC
    include
)