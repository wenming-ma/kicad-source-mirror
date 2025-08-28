# Core核心库

# 收集所有源文件
set(CORE_SRCS
    wx_stl_compat.cpp
)

# 创建静态库
add_library(core STATIC ${CORE_SRCS})

# 设置包含目录
target_include_directories(core PUBLIC
    include
)