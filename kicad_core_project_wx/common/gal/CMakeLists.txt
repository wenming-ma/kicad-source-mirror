# GAL library for minimum set

set( GAL_SRCS
    # Color support
    color4d.cpp
    cursors.cpp
    gal_display_options.cpp
    graphics_abstraction_layer.cpp
    painter.cpp
    
    # OpenGL backend (always available)
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
    
    # Bitmap font resources
    opengl/bitmap_font_desc.c
    opengl/bitmap_font_img.c
    
    # Generated GLSL shader resources
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_1_frag_color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_1_frag_luma.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_1_vert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_2_frag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_2_vert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_3_frag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_smaa_pass_3_vert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_kicad_frag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/common/gal/glsl_kicad_vert.cpp
)

# Conditionally add Cairo backend if available
if( CAIRO_FOUND )
    list( APPEND GAL_SRCS
        cairo/cairo_compositor.cpp
        cairo/cairo_gal.cpp
    )
    message( STATUS "Cairo GAL backend enabled" )
    if( PIXMAN_FOUND )
        message( STATUS "Cairo GAL backend will use Pixman for enhanced performance" )
    else()
        message( WARNING "Cairo GAL backend will run without Pixman (reduced performance)" )
    endif()
else()
    message( STATUS "Cairo GAL backend disabled - Cairo not found" )
endif()

add_library( gal STATIC ${GAL_SRCS} )

target_include_directories( gal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/kimath/include  # Added for math/vector2d.h, hash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/core/include    # Added for core/observable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/kiplatform/include  # Added for kiplatform/ui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../generated/include   # Added for generated GLSL shader headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../resources/bitmaps_png  # Added for cursor XPM files (cursors/cursor-*.xpm)
)

# Add Cairo include directories if available
if( CAIRO_FOUND )
    target_include_directories( gal PRIVATE ${CAIRO_INCLUDE_DIR} )
    # Add Pixman include directory only if Pixman was found
    if( PIXMAN_FOUND AND PIXMAN_INCLUDE_DIR )
        target_include_directories( gal PRIVATE ${PIXMAN_INCLUDE_DIR} )
    endif()
endif()

# Base target libraries
target_link_libraries( gal PUBLIC
    kimath      # Added for math library dependency
    core        # Added for core library dependency
    kiplatform  # Added for platform abstraction layer
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARIES}  # Added for OpenGL functionality
    ${GLEW_LIBRARIES}    # Added for GLEW OpenGL extension library
)

# Add Cairo and platform-specific libraries conditionally
if( CAIRO_FOUND )
    target_link_libraries( gal PUBLIC ${CAIRO_LIBRARIES} )
    
    # Add Pixman libraries only if Pixman was found
    if( PIXMAN_FOUND AND PIXMAN_LIBRARIES )
        target_link_libraries( gal PUBLIC ${PIXMAN_LIBRARIES} )
    endif()
    
    if( WIN32 )
        # GDI+ library needed for cairo printing on windows
        target_link_libraries( gal PUBLIC gdiplus )
    endif()
endif()

# Add compile definitions - for static library build, ensure GAL_API is empty
# We define SWIG to make GAL_API resolve to nothing (see gal.h logic)
target_compile_definitions( gal PUBLIC 
    SWIG  # This makes GAL_API empty in gal.h, avoiding DLL import/export decorations
)

# Add Cairo availability definition
if( CAIRO_FOUND AND PIXMAN_FOUND )
    target_compile_definitions( gal PRIVATE KICAD_GAL_CAIRO=1 )
    message( STATUS "Cairo GAL support: ENABLED" )
else()
    target_compile_definitions( gal PRIVATE KICAD_GAL_CAIRO=0 )
    message( STATUS "Cairo GAL support: DISABLED" )
endif()

# Note: Version header and git version are handled elsewhere in simplified build
# GLSL shader resources are now properly configured and included in the build