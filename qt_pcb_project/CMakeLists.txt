cmake_minimum_required(VERSION 3.16)
project(KiCadPCBTest)

set(CMAKE_CXX_STANDARD 20)

# 查找第三方依赖包
find_package(wxWidgets CONFIG REQUIRED)

# 添加第三方库
add_subdirectory(thirdparty)

# 添加include路径
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/kimath/include
    ${CMAKE_SOURCE_DIR}/libs/core/include
    ${CMAKE_SOURCE_DIR}/pcbnew
    ${CMAKE_SOURCE_DIR}/thirdparty/clipper2/include
    ${CMAKE_SOURCE_DIR}/thirdparty/clipper2/include/clipper2
)

# 先编译libs库
add_subdirectory(libs)

# 再编译common
add_subdirectory(common)

# 最后编译pcbnew (暂时注释)
# add_subdirectory(pcbnew)