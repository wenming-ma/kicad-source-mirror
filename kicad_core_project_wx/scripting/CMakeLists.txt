# Scripting library for minimum PCB set

add_library( scripting STATIC
    python_manager.cpp
    python_scripting.cpp
)

target_include_directories( scripting PUBLIC
    .
)

target_link_libraries( scripting PUBLIC
    common
)