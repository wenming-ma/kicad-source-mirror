# Kiplatform library for minimum PCB set

add_library( kiplatform STATIC
    os/windows/app.cpp
    os/windows/environment.cpp
    os/windows/io.cpp
    port/wxmsw/ui.cpp
)

target_include_directories( kiplatform PUBLIC
    include
    ${wxWidgets_INCLUDE_DIRS}
)

target_compile_definitions( kiplatform PUBLIC
    ${wxWidgets_DEFINITIONS}
)

target_link_libraries( kiplatform PUBLIC
    core
    ${wxWidgets_LIBRARIES}
    Shlwapi
    winhttp
    wintrust
    Imm32
)