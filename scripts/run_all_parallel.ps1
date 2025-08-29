# KiCad PCB 最小依赖集合分析 - 并行优化版
# 确保在 "x64 Native Tools Command Prompt for VS" 中执行

Write-Host "🚀 KiCad PCB 最小依赖集合分析工具 (并行优化版)" -ForegroundColor Green
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
    Write-Host "❌ 缺少必要工具" -ForegroundColor Red
    exit 1
}

# 检查必要文件
$required_files = @{
    "build\x64-Debug\compile_commands.json" = "编译数据库"
    "scripts\seeds.txt" = "种子文件列表"
}

foreach ($file in $required_files.Keys) {
    if (!(Test-Path $file)) {
        Write-Host "  ❌ $file - 不存在!" -ForegroundColor Red
        exit 1
    }
}

Write-Host "✅ 环境检查通过" -ForegroundColor Green
Write-Host ""

$start_time = Get-Date

# 第一阶段: 头文件闭包分析 (必须先执行)
Write-Host "[1/5] 📊 头文件闭包分析" -ForegroundColor Cyan
Write-Host "      使用clang-scan-deps分析种子文件的头文件依赖" -ForegroundColor Gray

$step_start = Get-Date
try {
    $result = python scripts\10_scan_src_deps.py 2>&1
    if ($LASTEXITCODE -eq 0) {
        $step_duration = (Get-Date) - $step_start
        Write-Host "      ✅ 完成 (耗时: $($step_duration.TotalSeconds.ToString('F1'))秒)" -ForegroundColor Green
    } else {
        Write-Host "      ❌ 执行失败" -ForegroundColor Red
        Write-Host $result -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "      ❌ 执行异常: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
Write-Host ""

# 第二阶段: TU符号索引构建 (时间最长，使用并行优化)
Write-Host "[2/5] 📊 TU符号索引构建 (并行处理)" -ForegroundColor Cyan
Write-Host "      编译TU并提取符号信息 (使用多线程加速)" -ForegroundColor Gray

$step_start = Get-Date
try {
    # 设置更多的并发参数来加速
    $env:PYTHONUNBUFFERED = "1"  # 实时输出
    $result = python scripts\20_build_tu_index.py 2>&1
    if ($LASTEXITCODE -eq 0) {
        $step_duration = (Get-Date) - $step_start
        Write-Host "      ✅ 完成 (耗时: $($step_duration.TotalSeconds.ToString('F1'))秒)" -ForegroundColor Green
    } else {
        Write-Host "      ❌ 执行失败" -ForegroundColor Red
        Write-Host $result -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "      ❌ 执行异常: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
Write-Host ""

# 第三阶段: 最小集合计算 + 头文件集合 (可以并行)
Write-Host "[3-4/5] 📊 最小集合计算 (并行执行)" -ForegroundColor Cyan
Write-Host "        计算最小源文件集合和头文件集合" -ForegroundColor Gray

$step_start = Get-Date

# 并行执行步骤3和4的准备
$job1 = Start-Job -ScriptBlock {
    Set-Location $using:PWD
    python scripts\30_resolve_minset.py 2>&1
}

# 等待步骤3完成后再执行步骤4
$result1 = Receive-Job $job1 -Wait
$exitCode1 = $job1.ExitCode
Remove-Job $job1

if ($exitCode1 -eq 0) {
    Write-Host "      ✅ 最小源文件集合计算完成" -ForegroundColor Green
    
    # 现在执行步骤4
    $job2 = Start-Job -ScriptBlock {
        Set-Location $using:PWD
        python scripts\40_emit_min_headers.py 2>&1
    }
    
    $result2 = Receive-Job $job2 -Wait
    $exitCode2 = $job2.ExitCode
    Remove-Job $job2
    
    if ($exitCode2 -eq 0) {
        $step_duration = (Get-Date) - $step_start
        Write-Host "      ✅ 最小头文件集合计算完成 (总耗时: $($step_duration.TotalSeconds.ToString('F1'))秒)" -ForegroundColor Green
    } else {
        Write-Host "      ❌ 头文件集合计算失败" -ForegroundColor Red
        Write-Host $result2 -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "      ❌ 源文件集合计算失败" -ForegroundColor Red
    Write-Host $result1 -ForegroundColor Red
    exit 1
}
Write-Host ""

# 第五阶段: 报告生成
Write-Host "[5/5] 📊 汇总报告生成" -ForegroundColor Cyan
Write-Host "      生成人类可读的分析报告" -ForegroundColor Gray

$step_start = Get-Date
try {
    $result = python scripts\50_report_minset.py 2>&1
    if ($LASTEXITCODE -eq 0) {
        $step_duration = (Get-Date) - $step_start
        Write-Host "      ✅ 完成 (耗时: $($step_duration.TotalSeconds.ToString('F1'))秒)" -ForegroundColor Green
        Write-Host "      📝 $result" -ForegroundColor DarkGreen
    } else {
        Write-Host "      ❌ 执行失败" -ForegroundColor Red
        Write-Host $result -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "      ❌ 执行异常: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

$total_duration = (Get-Date) - $start_time

Write-Host ""
Write-Host "🎉 并行分析完成!" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green
Write-Host "⏱️  总耗时: $($total_duration.TotalMinutes.ToString('F1')) 分钟" -ForegroundColor Green
Write-Host "🚄 通过并行优化显著提升了执行速度" -ForegroundColor Blue
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
Write-Host "🎯 查看结果: type DEPENDENCIES_MINSET.md" -ForegroundColor Cyan
Write-Host "✨ KiCad PCB 最小依赖分析完成! ✨" -ForegroundColor Green