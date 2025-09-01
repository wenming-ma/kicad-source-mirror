# Thirdparty libraries for minimum PCB set

# Clipper2
add_library( clipper2 STATIC
    clipper2/Clipper2Lib/src/clipper.engine.cpp
    clipper2/Clipper2Lib/src/clipper.offset.cpp
    clipper2/Clipper2Lib/src/clipper.rectclip.cpp
)

target_include_directories( clipper2 PUBLIC
    clipper2/Clipper2Lib/include
)

target_compile_definitions( clipper2 PUBLIC USINGZ )

if( MSVC )
    target_compile_options( clipper2 PRIVATE /W4 )
else()
    target_compile_options( clipper2 PRIVATE -Wall -Wextra -Wpedantic )
    target_link_libraries( clipper2 PUBLIC -lm )
endif()

# Delaunator
add_library( delaunator STATIC
    delaunator/delaunator.cpp
)

target_include_directories( delaunator PUBLIC
    delaunator
)

# FMT
add_library( fmt STATIC
    fmt/src/format.cc
)

target_include_directories( fmt PUBLIC
    fmt/include
)

if( MSVC )
    target_compile_options( fmt PRIVATE /utf-8 )
endif()

# JSON Schema Validator
add_library( json_schema_validator STATIC
    json_schema_validator/json-patch.cpp
    json_schema_validator/json-uri.cpp
    json_schema_validator/json-validator.cpp
)

target_include_directories( json_schema_validator PUBLIC
    json_schema_validator
    nlohmann_json
)

# Libcontext
add_library( libcontext STATIC
    libcontext/libcontext.cpp
)

target_include_directories( libcontext PUBLIC
    libcontext
)

# Other Math
add_library( other_math STATIC
    other_math/math_for_graphics.cpp
)

target_include_directories( other_math PUBLIC
    other_math
    ../libs/kimath/include
)

# Potrace
add_library( potrace STATIC
    potrace/src/curve.cpp
    potrace/src/decompose.cpp
    potrace/src/potracelib.cpp
    potrace/src/trace.cpp
)

target_include_directories( potrace PUBLIC
    potrace/include
)