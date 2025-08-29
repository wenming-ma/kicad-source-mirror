# KiCad PCB 最小依赖集合分析 - 一键执行脚本
# 确保在 "x64 Native Tools Command Prompt for VS" 中执行

Write-Host "🚀 KiCad PCB 最小依赖集合分析工具" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green
Write-Host ""

# 检查环境
Write-Host "🔧 检查分析环境..." -ForegroundColor Yellow

# 检查必要工具
$tools = @{
    "clang-scan-deps" = "LLVM依赖扫描工具"
    "llvm-nm" = "LLVM符号分析工具" 
    "cl.exe" = "MSVC编译器"
    "python" = "Python解释器"
}

$missing_tools = @()
foreach ($tool in $tools.Keys) {
    try {
        $null = Get-Command $tool -ErrorAction Stop
        Write-Host "  ✅ $tool - $($tools[$tool])" -ForegroundColor Green
    } catch {
        Write-Host "  ❌ $tool - $($tools[$tool]) 未找到!" -ForegroundColor Red
        $missing_tools += $tool
    }
}

if ($missing_tools.Count -gt 0) {
    Write-Host ""
    Write-Host "❌ 缺少必要工具，请确保在 'x64 Native Tools Command Prompt for VS' 中执行" -ForegroundColor Red
    Write-Host "   并安装 LLVM for Windows" -ForegroundColor Red
    exit 1
}

# 检查必要文件
Write-Host ""
Write-Host "📁 检查必要文件..." -ForegroundColor Yellow

$required_files = @{
    "build\x64-Debug\compile_commands.json" = "编译数据库"
    "scripts\seeds.txt" = "种子文件列表"
}

foreach ($file in $required_files.Keys) {
    if (Test-Path $file) {
        Write-Host "  ✅ $file - $($required_files[$file])" -ForegroundColor Green
    } else {
        Write-Host "  ❌ $file - $($required_files[$file]) 不存在!" -ForegroundColor Red
        if ($file -eq "build\x64-Debug\compile_commands.json") {
            Write-Host "     请先用CMake配置项目: cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..." -ForegroundColor Yellow
        }
        exit 1
    }
}

Write-Host ""
Write-Host "🎯 开始执行分析流水线..." -ForegroundColor Green
Write-Host ""

$steps = @(
    @{
        "script" = "scripts\10_scan_src_deps.py"
        "name" = "头文件闭包分析"
        "desc" = "使用clang-scan-deps分析种子文件的头文件依赖"
    },
    @{
        "script" = "scripts\20_build_tu_index.py"
        "name" = "TU符号索引构建"
        "desc" = "编译TU并提取符号信息"
    },
    @{
        "script" = "scripts\30_resolve_minset.py"
        "name" = "最小源文件集合计算"
        "desc" = "基于符号闭包算法计算最小依赖"
    },
    @{
        "script" = "scripts\40_emit_min_headers.py"
        "name" = "最小头文件集合计算"
        "desc" = "计算最小源文件集合的头文件闭包"
    },
    @{
        "script" = "scripts\50_report_minset.py"
        "name" = "汇总报告生成"
        "desc" = "生成人类可读的分析报告"
    }
)

$start_time = Get-Date
$step_count = 1

foreach ($step in $steps) {
    Write-Host "[$step_count/5] 📊 $($step.name)" -ForegroundColor Cyan
    Write-Host "      $($step.desc)" -ForegroundColor Gray
    
    $step_start = Get-Date
    
    try {
        $result = python $step.script
        if ($LASTEXITCODE -eq 0) {
            $step_duration = (Get-Date) - $step_start
            Write-Host "      ✅ 完成 (耗时: $($step_duration.TotalSeconds.ToString('F1'))秒)" -ForegroundColor Green
            if ($result) {
                Write-Host "      📝 $result" -ForegroundColor DarkGreen
            }
        } else {
            Write-Host "      ❌ 执行失败 (退出码: $LASTEXITCODE)" -ForegroundColor Red
            Write-Host "      错误详情:" -ForegroundColor Red
            Write-Host $result -ForegroundColor Red
            exit 1
        }
    } catch {
        Write-Host "      ❌ 执行异常: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
    
    $step_count++
    Write-Host ""
}

$total_duration = (Get-Date) - $start_time

Write-Host "🎉 分析完成!" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green
Write-Host "⏱️  总耗时: $($total_duration.TotalMinutes.ToString('F1')) 分钟" -ForegroundColor Green
Write-Host ""

# 显示结果文件
Write-Host "📊 生成的分析文件:" -ForegroundColor Yellow

$output_files = @{
    "build\x64-Debug\deps_src.json" = "种子文件头依赖闭包"
    "build\x64-Debug\tu_index.json" = "编译单元符号索引" 
    "build\x64-Debug\minset_sources.json" = "最小源文件集合"
    "build\x64-Debug\minset_headers.json" = "最小头文件集合"
    "build\x64-Debug\unresolved_symbols.json" = "未解析符号(外部依赖)"
    "DEPENDENCIES_MINSET.md" = "📋 人类可读汇总报告"
}

foreach ($file in $output_files.Keys) {
    if (Test-Path $file) {
        $size = [math]::Round((Get-Item $file).Length / 1KB, 1)
        Write-Host "  📄 $file ($size KB) - $($output_files[$file])" -ForegroundColor White
    } else {
        Write-Host "  ⚠️  $file - 未生成" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "🎯 下一步建议:" -ForegroundColor Cyan
Write-Host "  1. 查看分析报告: type DEPENDENCIES_MINSET.md" -ForegroundColor White
Write-Host "  2. 基于最小集合开始Qt移植工作" -ForegroundColor White
Write-Host "  3. 安装外部依赖: vcpkg install <packages>" -ForegroundColor White
Write-Host ""

# 显示统计摘要
if (Test-Path "build\x64-Debug\minset_sources.json") {
    $sources_data = Get-Content "build\x64-Debug\minset_sources.json" | ConvertFrom-Json
    $source_count = $sources_data.sources.Count
    Write-Host "📈 最小源文件集合: $source_count 个.cpp文件" -ForegroundColor Green
}

if (Test-Path "build\x64-Debug\minset_headers.json") {
    $headers_data = Get-Content "build\x64-Debug\minset_headers.json" | ConvertFrom-Json  
    $header_count = $headers_data.headers.Count
    Write-Host "📈 最小头文件集合: $header_count 个.h文件" -ForegroundColor Green
}

if (Test-Path "build\x64-Debug\unresolved_symbols.json") {
    $unresolved_data = Get-Content "build\x64-Debug\unresolved_symbols.json" | ConvertFrom-Json
    $unresolved_count = $unresolved_data.unresolved_symbols.Count  
    Write-Host "📈 未解析外部符号: $unresolved_count 个" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "✨ KiCad PCB 最小依赖分析完成! ✨" -ForegroundColor Green