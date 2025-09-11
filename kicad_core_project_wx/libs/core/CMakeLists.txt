# Core library for minimum PCB set

add_library( core STATIC
    observable.cpp
    utf8.cpp
    wx_stl_compat.cpp
)

target_link_libraries( core PUBLIC
    ${wxWidgets_LIBRARIES}
)

target_include_directories( core PUBLIC
    include
    ${wxWidgets_INCLUDE_DIRS}
)