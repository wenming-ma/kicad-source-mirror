#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"
SEEDS = ROOT / r"scripts\seeds-sch.txt"
OUT = ROOT / r"scripts\tem\minset_sources.json"
UNRES = ROOT / r"scripts\tem\unresolved_symbols.json"

# 扩展的外部库符号前缀，避免把系统/三方库当"缺失"
EXTERNAL_HINTS = [
    # C++标准库
    "std::",
    "stdext::",
    "__std",
    "__cxx",
    # 内存管理
    "operator new",
    "operator delete",
    "??_U",
    "??_V",
    "??2",
    "??3",
    # MSVC运行时
    "__imp_",
    "??_",
    "_CRT_",
    "__acrt_",
    "__vcrt_",
    "__security_",
    # Windows API
    "GetProcAddress",
    "LoadLibrary",
    "CreateFile",
    "RegQuery",
    "Nt",
    "Rtl",
    # 线程库
    "pthread_",
    "_beginthreadex",
    "CreateThread",
    "WaitForSingleObject",
    # 系统库
    "dlopen",
    "dlsym",
    "dlclose",
    "__libc_",
    "__glibc_",
    # Boost库
    "boost::",
    "__boost",
    "_boost_",
    # Google库
    "google::",
    "protobuf::",
    "__google_",
    # Qt框架
    "Qt",
    "Q",
    "qt_",
    "__qt_",
    # wxWidgets
    "wx",
    "WX",
    "WXDLLIMPEXP",
    "__wx_",
    # 加密库
    "SSL_",
    "EVP_",
    "CRYPTO_",
    "BN_",
    "RSA_",
    "AES_",
    "SHA1_",
    "SHA256_",
    # 数据库
    "sqlite3_",
    "SQLITE_",
    "mysql_",
    "postgres_",
    # 压缩库
    "zlib",
    "inflate",
    "deflate",
    "compress",
    "uncompress",
    # 图像库
    "png_",
    "jpeg_",
    "tiff_",
    "gif_",
    "webp_",
    # 网络库
    "curl_",
    "SSL_",
    "socket_",
    "inet_",
    # 其他常见第三方库
    "cairo_",
    "pango_",
    "glib_",
    "gtk_",
    "gdk_",
    # OpenGL相关
    "glew",
    "GLEW_",
    "gl",
    "GL_",
    "glm::",
    "glut",
    # 国际化库
    "gettext_",
    "libintl_",
    # 文本塑形库
    "hb_",
    "harfbuzz_",
    # CAD内核
    "OCC",
    "OpenCASCADE",
    "Standard_",
    "Handle_",
    # Unicode库
    "icu_",
    "U_",
    "UCHAR_",
    # 电路仿真
    "ng_",
    "spice_",
    # Git库
    "git_",
    # 消息库
    "nng_",
    # 压缩库扩展
    "ZSTD_",
    "zstd",
    # Python
    "Py",
    "PyObject",
    "_Py",
    # 编译器内建
    "__builtin_",
    "__clang",
    "__GNUC__",
    "_MSC_VER",
]


def is_external(sym):
    return any(sym.startswith(p) for p in EXTERNAL_HINTS)


def main():
    idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]
    per = {it["src"]: (set(it["defined"]), set(it["undefined"])) for it in idx_all}
    providers = {}
    for it in idx_all:
        for s in it["defined"]:
            providers.setdefault(s, set()).add(it["src"])

    seeds = {
        str((ROOT / s.strip()).resolve())
        for s in SEEDS.read_text(encoding="utf-8").splitlines()
        if s.strip() and not s.strip().startswith("#")
    }
    S = {s for s in seeds if s in per}
    print(f"Initial seed files: {len(S)} files")

    iteration = 0
    changed = True
    while changed:
        iteration += 1
        changed = False
        provided = set()
        required = set()

        # 收集当前集合的符号信息
        for s in S:
            d, u = per.get(s, (set(), set()))
            provided |= d
            required |= u

        # 找出缺失的内部符号
        missing = {m for m in (required - provided) if not is_external(m)}
        initial_missing_count = len(missing)

        print(
            f"Iteration {iteration}: size={len(S)}, provided={len(provided)}, required={len(required)}, missing={initial_missing_count}"
        )

        # 为缺失符号找提供者
        added_this_round = 0
        for m in sorted(missing):
            cands = providers.get(m, set()) - S
            if cands:
                chosen = sorted(cands)[0]  # 选择字典序最小的候选者
                S.add(chosen)
                added_this_round += 1
                changed = True

        if added_this_round > 0:
            print(f"  -> Added {added_this_round} new source files")

        # 防止无限循环
        if iteration > 100:
            print("Warning: Maximum iterations reached, possible circular dependency")
            break

    print(f"Symbol closure converged, final set size: {len(S)} source files")

    provided = set()
    required = set()
    for s in S:
        d, u = per.get(s, (set(), set()))
        provided |= d
        required |= u
    unresolved = sorted(required - provided)

    # Ensure output directory exists
    OUT.parent.mkdir(parents=True, exist_ok=True)

    OUT.write_text(json.dumps({"sources": sorted(S)}, indent=2), encoding="utf-8")
    UNRES.write_text(
        json.dumps({"unresolved_symbols": unresolved}, indent=2), encoding="utf-8"
    )
    print(f"Wrote {OUT}; unresolved={len(unresolved)}")


if __name__ == "__main__":
    main()
