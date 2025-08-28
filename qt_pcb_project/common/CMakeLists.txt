# Common通用库

set(COMMON_SRCS
    eda_item.cpp
    eda_shape.cpp
    eda_text.cpp
    kiid.cpp
    lib_id.cpp
    lset.cpp
    netclass.cpp
)

# 创建静态库
add_library(common STATIC ${COMMON_SRCS})

# 链接依赖库
target_link_libraries(common
    kimath
    core
)

# 设置包含目录
target_include_directories(common PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)