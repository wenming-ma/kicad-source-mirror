# Kinng library for minimum PCB set

add_library( kinng STATIC
    src/kinng.cpp
)

target_include_directories( kinng PUBLIC
    include
)