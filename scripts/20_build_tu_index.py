#!/usr/bin/env python3
import json
import shlex
import subprocess
import time
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import multiprocessing
import os

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / r"build\x64-Debug"
CCDB = BUILD / "compile_commands.json"
OBJ = BUILD / "objcache"
OUT = BUILD / "tu_index.json"


def iso():
    return time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())


def run(cmd, cwd=None):
    return subprocess.run(cmd, cwd=cwd, text=True, capture_output=True)


def strip_to_compile(argv, src_to_exclude=None):
    """移除输出相关参数和源文件，保留编译必需的参数"""
    out = []
    skip = False
    i = 0
    while i < len(argv):
        arg = argv[i]
        
        if skip:
            skip = False
            i += 1
            continue
            
        # 跳过已有的 /c 参数
        if arg in ("-c", "/c"):
            i += 1
            continue
            
        # 跳过输出文件参数
        if arg in ("-o", "/Fo"):
            skip = True  # 下一个参数也要跳过
            i += 1
            continue
            
        # 跳过以/Fo开头的参数（如 /FoFile.obj）
        if arg.startswith("/Fo"):
            i += 1
            continue
            
        # 跳过以/Fd开头的参数（PDB文件）
        if arg.startswith("/Fd"):
            i += 1
            continue
            
        # 跳过源文件参数（通常是最后一个，但也可能在中间）
        if src_to_exclude and arg == src_to_exclude:
            i += 1
            continue
            
        # 跳过看起来像源文件的参数
        if arg.endswith(('.c', '.cc', '.cpp', '.cxx', '.mm', '.m')) and '\\' in arg:
            i += 1
            continue
            
        # 保留其他所有参数（包括头文件路径、宏定义等）
        out.append(arg)
        i += 1
    
    return out


def deps_for_src(cmd, cwd, src_abs):
    dep_cmd = cmd + ["-M", "-MG", "-MF", "-", src_abs]
    p = run(dep_cmd, cwd)
    deps = set()
    txt = p.stdout.replace("\\\n", " ")
    import shlex as _sh

    for line in txt.splitlines():
        if ":" not in line:
            continue
        rhs = line.split(":", 1)[1]
        for tok in _sh.split(rhs):
            if tok.strip():
                deps.add(str(Path(tok).resolve()))
    return sorted(deps)


def nm_defined(obj):
    p = run(["llvm-nm", "-g", "--defined-only", obj])
    syms = set()
    for line in p.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3:
            syms.add(parts[-1])
    return sorted(syms)


def nm_undefined(obj):
    p = run(["llvm-nm", "-u", obj])
    syms = set()
    for line in p.stdout.splitlines():
        s = line.strip()
        if s:
            syms.add(s.split()[-1])
    return sorted(syms)


def process_tu(entry):
    """处理单个编译单元"""
    e = entry
    cwd = Path(e["directory"])
    # 优先使用arguments字段，如果没有则手动解析command
    if "arguments" in e:
        argv = e["arguments"]
        # 将完整路径的编译器替换为简单的cl.exe
        if argv and argv[0].endswith("cl.exe"):
            argv = ["cl.exe"] + argv[1:]
    else:
        # 从command字段中提取，但使用cl.exe
        argv = ["cl.exe"] + e["command"].split()[1:]  # 跳过第一个参数（完整路径的编译器）
    src = Path(e["file"])
    src_abs = str((cwd / src).resolve())
    
    if not src_abs.endswith((".c", ".cc", ".cpp", ".cxx", ".mm", ".m")):
        return None

    base = Path(src_abs).name
    objp = str((OBJ / (base + ".obj")).resolve())
    
    # 编译到 objcache（保持与原 TU 相同宏/包含）
    # 使用原始编译单元的工作目录
    cleaned_argv = strip_to_compile(argv, src_abs)
    cmd = cleaned_argv + ["/c", src_abs, f"/Fo{objp}"]
    # 使用原始编译单元的工作目录
    r = run(cmd, cwd)
    if r.returncode != 0:
        print(f"[warn] compile failed: {src_abs}\n{r.stderr[:200]}...")

    headers = []
    try:
        headers = deps_for_src(strip_to_compile(argv), cwd, src_abs)
    except Exception as ex:
        print(f"[warn] deps failed: {src_abs} -> {ex}")

    defined = nm_defined(objp) if Path(objp).exists() else []
    undefined = nm_undefined(objp) if Path(objp).exists() else []

    return {
        "src": src_abs,
        "obj": objp,
        "defined": defined,
        "undefined": undefined,
        "headers": headers,
        "cwd": str(cwd),
        "argv": strip_to_compile(argv),
    }

def main():
    OBJ.mkdir(parents=True, exist_ok=True)
    comp = json.loads(CCDB.read_text(encoding="utf-8"))
    
    # 只处理C/C++文件
    cpp_entries = [e for e in comp 
                   if str(Path(e["directory"]) / Path(e["file"])).endswith((".c", ".cc", ".cpp", ".cxx", ".mm", ".m"))]
    
    print(f"处理 {len(cpp_entries)} 个C/C++编译单元...")
    
    # 使用线程池并行处理（I/O密集型任务）
    max_workers = min(multiprocessing.cpu_count() * 2, len(cpp_entries))
    print(f"使用 {max_workers} 个并发线程...")
    
    items = []
    completed_count = 0
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        # 提交所有任务
        future_to_entry = {executor.submit(process_tu, entry): entry for entry in cpp_entries}
        
        # 收集结果
        for future in as_completed(future_to_entry):
            result = future.result()
            if result:
                items.append(result)
            
            completed_count += 1
            if completed_count % 50 == 0:
                print(f"进度: {completed_count}/{len(cpp_entries)} ({completed_count/len(cpp_entries)*100:.1f}%)")

    print(f"完成处理 {len(items)} 个有效编译单元")
    
    OUT.write_text(
        json.dumps({"generated_at": iso(), "items": items}, indent=2), encoding="utf-8"
    )
    print(f"Wrote {OUT} (TUs={len(items)})")


if __name__ == "__main__":
    main()
