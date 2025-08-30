#!/usr/bin/env python3
import subprocess
import os
from pathlib import Path

# 设置工作目录
os.chdir(r"D:\MyProjects\kicad\kicad\build")

# 编译命令
cmd = [
    r"C:\PROGRA~1\MICROS~1\2022\COMMUN~1\VC\Tools\MSVC\1444~1.352\bin\Hostx64\x64\cl.exe",
    "/nologo", "/TP",
    "-DBOOST_ALL_NO_LIB", "-DBOOST_LOCALE_DYN_LINK", "-DBOOST_LOCALE_NO_LIB", "-DBOOST_UUID_FORCE_AUTO_LINK",
    "-DDEBUG", "-DGLM_FORCE_CTOR_INIT", "-DHAVE_STDINT_H", "-DJSON_HAS_FILESYSTEM=0",
    "-DKIAPI_IMPORTEXPORT=APIIMPORT", "-DKICAD_BUILD_ARCH=x64", "-DKICAD_BUILD_ARCH_X64",
    "-DKICAD_CONFIG_DIR=kicad", "-DKICAD_IPC_API", "-DKICAD_SCRIPTING_WXPYTHON",
    "-DKICAD_UPDATE_CHECK", "-DKICAD_WIN32_DPI_AWARE=1", "-DNANODBC_ENABLE_UNICODE",
    "-DNOMINMAX", "-DPROTOBUF_USE_DLLS", "-DUNICODE", "-DUSINGZ", "-DWIN32_LEAN_AND_MEAN",
    "-D_CRT_NONSTDC_NO_DEPRECATE", "-D_CRT_SECURE_NO_WARNINGS", "-D_SCL_SECURE_NO_WARNINGS",
    "-D_UNICODE", "-D_USE_MATH_DEFINES", "-DPCBNEW",
    r"-ID:\MyProjects\kicad\kicad\include",
    r"-ID:\MyProjects\kicad\kicad\common\.",
    r"-ID:\MyProjects\kicad\kicad\resources\msw",
    r"-ID:\MyProjects\kicad\kicad\common\.\dialogs",
    r"-ID:\MyProjects\kicad\kicad\common\.\widgets",
    r"-ID:\MyProjects\kicad\kicad\common\.\dialog_about",
    r"-ID:\MyProjects\kicad\kicad\resources\bitmaps_png",
    r"-ID:\MyProjects\kicad\kicad\3d-viewer",
    r"-ID:\MyProjects\kicad\kicad\pcbnew",
    r"-ID:\MyProjects\kicad\kicad\kicad",
    r"-ID:\MyProjects\kicad\kicad\build",
    r"-ID:\MyProjects\kicad\kicad\scripting",
    r"-ID:\MyProjects\kicad\kicad\build\common",
    r"-ID:\MyProjects\kicad\kicad\libs\core\include",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\argparse\include",
    r"-ID:\MyProjects\kicad\kicad\build\api\cpp",
    r"-ID:\MyProjects\kicad\kicad\build\api\cpp\api",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\pegtl",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\magic_enum\magic_enum",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\libcontext",
    r"-ID:\MyProjects\kicad\kicad\libs\kimath\include",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\clipper2\Clipper2Lib\include",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\other_math",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\rtree",
    r"-ID:\MyProjects\kicad\kicad\libs\kiplatform\include",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\dynamic_bitset",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\libpopcnt",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\expected\include",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\picosha2",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\json_schema_validator",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\nlohmann_json",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\thread-pool",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\fmt\include",
    r"-ID:\MyProjects\kicad\kicad\libs\kinng\include",
    r"-ID:\MyProjects\kicad\kicad\build\include\gal\shaders",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\nanosvg",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\dxflib_qcad",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\tinyspline_lib",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\compoundfilereader",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\delaunator",
    r"-ID:\MyProjects\kicad\kicad\thirdparty\3dxware_sdk\inc",
    r"-external:ID:\MyProjects\kicad\kicad\thirdparty\pybind11\include",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\include",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\include\cairo",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\include\pixman-1",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\include\harfbuzz",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\include\opencascade",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\lib\mswu",
    r"-external:ID:\MyProjects\kicad\kicad\build\vcpkg_installed\x64-windows\include\python3.11",
    r"-external:ID:\MyProjects\kicad\kicad\thirdparty\nanodbc",
    "-external:W0",
    "/DWIN32", "/D_WINDOWS", "/utf-8", "/Zc:__cplusplus", "/Zc:inline", "/bigobj",
    "/permissive-", "/EHs", "/external:W0", "/external:env:INCLUDE",
    r"/external:ID:/MyProjects/kicad/kicad/thirdparty",
    "/Wall", "/wd4251", "/wd4710", "/wd4711", "/wd4820", "/wd4100", "/wd4623", "/wd4625",
    "/wd5026", "/wd4626", "/wd5027", "/wd5045", "/wd4061", "/wd4245", "/wd4365", "/wd4242",
    "/wd5219", "/wd4263", "/wd4264", "/wd4266", "/wd5204", "/wd4265", "/wd4371", "/wd4464",
    "/wd5264", "/wd5262", "/wd4514", "/wd4868", "/wd4866", "/wd4668", "/wd5267", "/wd4946",
    "/wd4290", "/wd4800", "/wd4746",
    "/Ob0", "/Od", "/RTC1", "-std:c++20", "-MDd", "-Zi", "/utf-8",
    r"/Fdcommon\CMakeFiles\pcbcommon.dir\pcbcommon.pdb",
    "/FS",
    "/showIncludes", "/c", "/Fonul",
    r"D:\MyProjects\kicad\kicad\pcbnew\board.cpp"
]

print("Executing command to get board.cpp dependencies...")
print(f"Working directory: {os.getcwd()}")

# 执行命令
result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace')

# 保存输出
output_file = Path("board_deps_output.txt")
with open(output_file, "w", encoding="utf-8") as f:
    f.write("=== STDOUT ===\n")
    f.write(result.stdout)
    f.write("\n\n=== STDERR ===\n")
    f.write(result.stderr)
    f.write(f"\n\n=== RETURN CODE: {result.returncode} ===\n")

print(f"Output saved to: {output_file.absolute()}")
print(f"Return code: {result.returncode}")
print(f"Output lines: {len(result.stdout.splitlines())}")
print(f"Error lines: {len(result.stderr.splitlines())}")

# Count Note: including file: occurrences
include_count = sum(1 for line in result.stdout.splitlines() if "Note: including file:" in line)
print(f"Number of included headers: {include_count}")