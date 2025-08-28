# KiMath数学几何库

# 收集所有源文件
set(KIMATH_SRCS
    src/bezier_curves.cpp
    src/convert_basic_shapes_to_polygon.cpp
    src/trigo.cpp
    src/geometry/convex_hull.cpp
    src/geometry/eda_angle.cpp
    src/geometry/geometry_utils.cpp
    src/geometry/seg.cpp
    src/geometry/shape.cpp
    src/geometry/shape_arc.cpp
    src/geometry/shape_compound.cpp
    src/geometry/shape_line_chain.cpp
    src/geometry/shape_poly_set.cpp
    src/geometry/shape_rect.cpp
    src/geometry/shape_segment.cpp
    src/math/util.cpp
    src/math/vector2.cpp
)

# 创建静态库
add_library(kimath STATIC ${KIMATH_SRCS})

# 设置包含目录
target_include_directories(kimath PUBLIC
    include
)