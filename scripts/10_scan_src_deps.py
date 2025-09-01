#!/usr/bin/env python3
import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description="扫描源文件依赖")
    parser.add_argument(
        "-b", "--build", default="build/x64-Debug", help="构建目录路径 (默认: build)"
    )
    parser.add_argument(
        "-s",
        "--seeds",
        default=str(ROOT / r"scripts\seeds.txt"),
        help="种子文件路径 (默认: scripts/seeds.txt)",
    )
    args = parser.parse_args()

    # 设置路径
    BUILD = ROOT / args.build
    CCDB = BUILD / "compile_commands.json"
    SCAN = BUILD / "deps_full.json"
    SEEDS = Path(args.seeds)
    OUT = BUILD / "deps_src.json"

    # 检查编译数据库是否存在
    if not CCDB.exists():
        print(f"错误: 编译数据库不存在: {CCDB}")
        print(f"请先运行CMake生成compile_commands.json:")
        print(f"  cmake -B {args.build} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
        sys.exit(1)

    # 检查种子文件是否存在
    if not SEEDS.exists():
        print(f"错误: 种子文件不存在: {SEEDS}")
        sys.exit(1)
    # 1) 调用 clang-scan-deps
    import os

    cmd = [
        "clang-scan-deps",
        f"-compilation-database={CCDB}",
        "-format=experimental-full",
        "-j",
        "0",  # 使用所有可用CPU核心
    ]

    # 检查是否已经存在deps_full.json
    if SCAN.exists():
        print(f"发现已存在的依赖数据: {SCAN}")
        file_size = SCAN.stat().st_size / (1024 * 1024)  # MB
        print(f"文件大小: {file_size:.1f} MB")

        # 询问是否要重新生成
        import time

        file_age = (time.time() - SCAN.stat().st_mtime) / 3600  # 小时
        print(f"文件年龄: {file_age:.1f} 小时")

        if file_age < 1:  # 1小时内的文件直接使用
            print("使用现有的依赖数据（文件较新）")
        else:
            print("警告: 依赖数据文件较旧，建议重新生成")
    else:
        print(f"执行命令: {' '.join(cmd)}")

        # 执行clang-scan-deps
        result = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8")

        if result.returncode != 0:
            print(f"警告: clang-scan-deps退出码为 {result.returncode}")
            print(f"stderr: {result.stderr[:500]}...")  # 显示前500字符的错误

        # 检查是否有有效的stdout输出
        if result.stdout.strip():
            print(f"成功获取到依赖数据，长度: {len(result.stdout)} 字符")
            SCAN.write_text(result.stdout, encoding="utf-8")
        else:
            print("错误: 没有获取到有效的依赖数据")
            if result.stderr:
                print(f"完整错误信息:\n{result.stderr}")
            sys.exit(1)

    seeds = {
        str((ROOT / s.strip()).resolve())
        for s in SEEDS.read_text(encoding="utf-8").splitlines()
        if s.strip() and not s.strip().startswith("#")
    }
    data = json.loads(SCAN.read_text(encoding="utf-8"))
    items = []
    processed_count = 0
    valid_tu_count = 0

    for tu in data.get("translation-units", []):
        processed_count += 1
        if processed_count % 1000 == 0:
            print(
                f"处理进度: {processed_count}/{len(data.get('translation-units', []))}"
            )

        # clang-scan-deps格式：从commands数组中提取信息
        if "commands" not in tu or not tu["commands"]:
            continue

        cmd_info = tu["commands"][0]  # 取第一个command

        # 源文件在input-file字段中
        src = cmd_info.get("input-file")

        # 文件依赖在file-deps字段中
        file_deps = cmd_info.get("file-deps", [])

        # 跳过无效的编译单元
        if not src:
            continue

        try:
            # src已经是绝对路径
            tu_src = str(Path(src).resolve())
        except Exception as e:
            print(f"警告: 无法解析路径 {src}: {e}")
            continue

        # 检查是否匹配种子文件（支持相对路径匹配）
        is_seed = False
        for seed_abs in seeds:
            if tu_src == seed_abs:
                is_seed = True
                break

        if not is_seed:
            continue

        valid_tu_count += 1
        print(f"处理种子文件: {tu_src}")

        # 处理文件依赖
        for p in file_deps:
            if not p:  # 跳过空路径
                continue

            try:
                ext = Path(p).suffix.lower()
                cat = (
                    "header"
                    if ext in (".h", ".hpp", ".hh", ".hxx", ".inc", ".inl")
                    else "source"
                )
                items.append(
                    {
                        "category": cat,
                        "path": str(Path(p).resolve()),
                        "from": "clang-scan-deps",
                        "tu": tu_src,
                    }
                )
            except Exception as e:
                print(f"警告: 无法处理依赖路径 {p}: {e}")
                continue

        # 添加源文件本身
        items.append(
            {"category": "source", "path": tu_src, "from": "seed", "tu": tu_src}
        )

    print(
        f"处理完成: 总TU={processed_count}, 有效种子TU={valid_tu_count}, 依赖项={len(items)}"
    )
    OUT.write_text(
        json.dumps(
            {"meta": {"tool": "clang-scan-deps", "build": str(BUILD)}, "items": items},
            indent=2,
        ),
        encoding="utf-8",
    )
    print(f"Wrote {OUT}")


if __name__ == "__main__":
    main()
