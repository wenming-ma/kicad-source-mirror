# GAL library for minimum set

set( GAL_SRCS
    # Color support
    color4d.cpp
    cursors.cpp
    graphics_abstraction_layer.cpp
    painter.cpp
    
    # Cairo backend
    cairo/cairo_compositor.cpp
    cairo/cairo_gal.cpp
    
    # OpenGL backend
    hidpi_gl_canvas.cpp
    opengl/antialiasing.cpp
    opengl/cached_container.cpp
    opengl/cached_container_gpu.cpp
    opengl/cached_container_ram.cpp
    opengl/gl_context_mgr.cpp
    opengl/gl_resources.cpp
    opengl/gpu_manager.cpp
    opengl/noncached_container.cpp
    opengl/opengl_compositor.cpp
    opengl/opengl_gal.cpp
    opengl/shader.cpp
    opengl/utils.cpp
    opengl/vertex_container.cpp
    opengl/vertex_item.cpp
    opengl/vertex_manager.cpp
)

add_library( gal STATIC ${GAL_SRCS} )

target_include_directories( gal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CAIRO_INCLUDE_DIR}
)

if( WIN32 )
    # GDI+ library needed for cairo printing on windows
    set( GDI_PLUS_LIBRARIES gdiplus )
endif()

target_link_libraries( gal PUBLIC
    ${wxWidgets_LIBRARIES}
    ${GDI_PLUS_LIBRARIES}
    ${CAIRO_LIBRARIES}
)

# Add compile definitions
target_compile_definitions( gal PRIVATE 
    GAL_DLL=1 
)

# Note: Version header and git version are handled elsewhere in simplified build
# Shader compilation is disabled for minimum set