# Kimath library for minimum PCB set

add_library( kimath STATIC
    src/bezier_curves.cpp
    src/convert_basic_shapes_to_polygon.cpp
    src/trigo.cpp
    src/geometry/circle.cpp
    src/geometry/convex_hull.cpp
    src/geometry/eda_angle.cpp
    src/geometry/geometry_utils.cpp
    src/geometry/half_line.cpp
    src/geometry/line.cpp
    src/geometry/seg.cpp
    src/geometry/shape.cpp
    src/geometry/shape_arc.cpp
    src/geometry/shape_collisions.cpp
    src/geometry/shape_compound.cpp
    src/geometry/shape_line_chain.cpp
    src/geometry/shape_poly_set.cpp
    src/geometry/shape_rect.cpp
    src/geometry/shape_segment.cpp
    src/geometry/shape_utils.cpp
    src/geometry/vertex_set.cpp
    src/math/util.cpp
    src/math/vector2.cpp
)

target_include_directories( kimath PUBLIC
    include
)

target_link_libraries( kimath PUBLIC
    core
    clipper2
)