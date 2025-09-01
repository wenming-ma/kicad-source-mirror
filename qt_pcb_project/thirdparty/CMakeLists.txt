# Thirdparty libraries for minimum PCB set

# Clipper2
add_library( clipper2 STATIC
    clipper2/Clipper2Lib/src/clipper.engine.cpp
    clipper2/Clipper2Lib/src/clipper.offset.cpp
)

target_include_directories( clipper2 PUBLIC
    clipper2/Clipper2Lib/include
)

# Delaunator
add_library( delaunator STATIC
    delaunator/delaunator.cpp
)

target_include_directories( delaunator PUBLIC
    delaunator/include
)

# FMT
add_library( fmt STATIC
    fmt/src/format.cc
)

target_include_directories( fmt PUBLIC
    fmt/include
)

# JSON Schema Validator
add_library( json_schema_validator STATIC
    json_schema_validator/json-patch.cpp
    json_schema_validator/json-uri.cpp
    json_schema_validator/json-validator.cpp
)

target_include_directories( json_schema_validator PUBLIC
    json_schema_validator/include
    nlohmann_json/include
)

# Libcontext
add_library( libcontext STATIC
    libcontext/libcontext.cpp
)

target_include_directories( libcontext PUBLIC
    libcontext/include
)

# Other Math
add_library( other_math STATIC
    other_math/math_for_graphics.cpp
)

target_include_directories( other_math PUBLIC
    other_math/include
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