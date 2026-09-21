param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    [switch]$Run,
    [switch]$Test
)

$ErrorActionPreference = "Stop"

$simDir = $PSScriptRoot
$gfxDir = Split-Path -Parent $simDir
$srcDir = Join-Path $gfxDir "src"
$buildDir = Join-Path $simDir "build"
$outputPath = Join-Path $buildDir "xgfx_sim.exe"
$testSource = Join-Path $gfxDir "tests\regression_tests.c"
$testOutputPath = Join-Path $buildDir "xgfx_regression_tests.exe"

$requiredFiles = @(
    (Join-Path $srcDir "GFX.c"),
    (Join-Path $srcDir "Area.c"),
    (Join-Path $simDir "GFX_Port_Sim.c"),
    (Join-Path $simDir "GFX_Sim.c"),
    (Join-Path $simDir "main.c")
)

foreach ($file in $requiredFiles) {
    if (-not (Test-Path -LiteralPath $file -PathType Leaf)) {
        throw "Missing build input: $file"
    }
}

$compilerCandidates = @("gcc", "E:\mingw64\bin\gcc.exe")
$compiler = $null
foreach ($candidate in $compilerCandidates) {
    $command = Get-Command $candidate -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        $compiler = $command.Source
        break
    }
}

if ($null -eq $compiler) {
    throw "GCC was not found. Install MinGW-w64 or add gcc to PATH."
}

$compilerDir = Split-Path -Parent $compiler
if (($env:Path -split ";") -notcontains $compilerDir) {
    $env:Path = "$compilerDir;$env:Path"
}

New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$compilerArgs = @(
    "-std=c11",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-mwindows",
    "-I$srcDir",
    "-I$simDir"
)

if ($Configuration -eq "Debug") {
    $compilerArgs += @("-O0", "-g3")
} else {
    $compilerArgs += @("-O2", "-DNDEBUG")
}

$compilerArgs += $requiredFiles
$compilerArgs += @("-o", $outputPath, "-lgdi32", "-luser32")

Write-Host "[INFO] Configuration: $Configuration"
Write-Host "[INFO] Compiler: $compiler"
Write-Host "[INFO] Output: $outputPath"

& $compiler @compilerArgs
if ($LASTEXITCODE -ne 0) {
    throw "XGFX simulator build failed. GCC exit code: $LASTEXITCODE"
}

Write-Host "[OK] XGFX simulator build succeeded" -ForegroundColor Green

if ($Run) {
    Write-Host "[INFO] Starting simulator"
    & $outputPath
}

if ($Test) {
    if (-not (Test-Path -LiteralPath $testSource -PathType Leaf)) {
        throw "Missing regression test source: $testSource"
    }

    $testArgs = @(
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-mconsole",
        "-I$srcDir",
        "-I$simDir"
    )
    if ($Configuration -eq "Debug") {
        $testArgs += @("-O0", "-g3")
    } else {
        $testArgs += @("-O2", "-DNDEBUG")
    }
    $testArgs += @(
        (Join-Path $srcDir "GFX.c"),
        (Join-Path $srcDir "Area.c"),
        (Join-Path $simDir "GFX_Port_Sim.c"),
        $testSource,
        "-o", $testOutputPath
    )

    Write-Host "[INFO] Building regression tests"
    & $compiler @testArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Regression test build failed. GCC exit code: $LASTEXITCODE"
    }

    Write-Host "[INFO] Running regression tests"
    Push-Location $simDir
    try {
        & $testOutputPath
        $testExitCode = $LASTEXITCODE
    } finally {
        Pop-Location
    }
    if ($testExitCode -ne 0) {
        throw "Regression tests failed. Exit code: $testExitCode"
    }
    Write-Host "[OK] All regression tests passed" -ForegroundColor Green
}
