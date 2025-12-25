# Run desktop builds of the automotive UI suite (Windows)
# Usage: .\run_desktop.ps1 [driver|infotainment|both]

param(
    [Parameter(Position=0)]
    [ValidateSet('driver', 'infotainment', 'both', 'build', 'test', 'clean', 'help')]
    [string]$Command = 'both'
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectRoot "build"

function Write-Header {
    Write-Host "========================================" -ForegroundColor Blue
    Write-Host "  Automotive UI Suite - Desktop Runner" -ForegroundColor Blue
    Write-Host "========================================" -ForegroundColor Blue
    Write-Host ""
}

function Write-Step {
    param([string]$Message)
    Write-Host "[*] $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[!] $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Check-Dependencies {
    Write-Step "Checking dependencies..."

    # Check for CMake
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Error "CMake is not installed. Please install CMake 3.21+."
        exit 1
    }

    # Check for Visual Studio / MSVC
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vsWhere)) {
        Write-Warning "Visual Studio not found. Make sure you have VS 2019 or later installed."
    }

    Write-Step "Dependencies OK"
}

function Configure-Build {
    Write-Step "Configuring build..."

    if (-not (Test-Path $BuildDir)) {
        New-Item -ItemType Directory -Path $BuildDir | Out-Null
    }

    Push-Location $BuildDir
    try {
        cmake $ProjectRoot `
            -DCMAKE_BUILD_TYPE=Debug `
            -DBUILD_DRIVER_UI=ON `
            -DBUILD_INFOTAINMENT_UI=ON `
            -DENABLE_SIM=ON `
            -DENABLE_TESTS=ON
    }
    finally {
        Pop-Location
    }

    Write-Step "Configuration complete"
}

function Build-Project {
    Write-Step "Building project..."

    Push-Location $BuildDir
    try {
        cmake --build . --config Debug --parallel
    }
    finally {
        Pop-Location
    }

    Write-Step "Build complete"
}

function Start-DriverUI {
    Write-Step "Starting Driver UI (Cluster)..."

    $DriverExe = Join-Path $BuildDir "driver_ui\Debug\driver_ui.exe"
    if (-not (Test-Path $DriverExe)) {
        $DriverExe = Join-Path $BuildDir "driver_ui\driver_ui.exe"
    }

    if (Test-Path $DriverExe) {
        $script:DriverProcess = Start-Process -FilePath $DriverExe -PassThru
        Write-Host "Driver UI started (PID: $($script:DriverProcess.Id))"
    }
    else {
        Write-Error "Driver UI binary not found. Did the build succeed?"
        return $false
    }
    return $true
}

function Start-InfotainmentUI {
    Write-Step "Starting Infotainment UI..."

    $InfoExe = Join-Path $BuildDir "infotainment_ui\Debug\infotainment_ui.exe"
    if (-not (Test-Path $InfoExe)) {
        $InfoExe = Join-Path $BuildDir "infotainment_ui\infotainment_ui.exe"
    }

    if (Test-Path $InfoExe) {
        $script:InfoProcess = Start-Process -FilePath $InfoExe -PassThru
        Write-Host "Infotainment UI started (PID: $($script:InfoProcess.Id))"
    }
    else {
        Write-Error "Infotainment UI binary not found. Did the build succeed?"
        return $false
    }
    return $true
}

function Run-Tests {
    Write-Step "Running tests..."

    Push-Location $BuildDir
    try {
        ctest --output-on-failure -C Debug
    }
    finally {
        Pop-Location
    }

    Write-Step "Tests complete"
}

function Show-Usage {
    Write-Host "Usage: .\run_desktop.ps1 [OPTION]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  driver        Build and run Driver UI only"
    Write-Host "  infotainment  Build and run Infotainment UI only"
    Write-Host "  both          Build and run both UIs (default)"
    Write-Host "  build         Build without running"
    Write-Host "  test          Build and run tests"
    Write-Host "  clean         Clean build directory"
    Write-Host "  help          Show this help message"
    Write-Host ""
    Write-Host "Environment Variables:"
    Write-Host "  CMAKE_PREFIX_PATH   Path to Qt installation"
    Write-Host ""
}

function Stop-Processes {
    Write-Step "Cleaning up..."
    if ($script:DriverProcess -and -not $script:DriverProcess.HasExited) {
        Stop-Process -Id $script:DriverProcess.Id -Force -ErrorAction SilentlyContinue
    }
    if ($script:InfoProcess -and -not $script:InfoProcess.HasExited) {
        Stop-Process -Id $script:InfoProcess.Id -Force -ErrorAction SilentlyContinue
    }
}

# Main script
Write-Header

try {
    switch ($Command) {
        'driver' {
            Check-Dependencies
            Configure-Build
            Build-Project
            if (Start-DriverUI) {
                Write-Host ""
                Write-Step "Press Ctrl+C to stop..."
                $script:DriverProcess.WaitForExit()
            }
        }
        'infotainment' {
            Check-Dependencies
            Configure-Build
            Build-Project
            if (Start-InfotainmentUI) {
                Write-Host ""
                Write-Step "Press Ctrl+C to stop..."
                $script:InfoProcess.WaitForExit()
            }
        }
        'both' {
            Check-Dependencies
            Configure-Build
            Build-Project
            $driverOk = Start-DriverUI
            Start-Sleep -Seconds 1
            $infoOk = Start-InfotainmentUI
            if ($driverOk -or $infoOk) {
                Write-Host ""
                Write-Step "Press Ctrl+C to stop both UIs..."
                if ($script:DriverProcess) { $script:DriverProcess.WaitForExit() }
            }
        }
        'build' {
            Check-Dependencies
            Configure-Build
            Build-Project
            Write-Step "Build complete. Run '.\run_desktop.ps1' to start UIs."
        }
        'test' {
            Check-Dependencies
            Configure-Build
            Build-Project
            Run-Tests
        }
        'clean' {
            Write-Step "Cleaning build directory..."
            if (Test-Path $BuildDir) {
                Remove-Item -Recurse -Force $BuildDir
            }
            Write-Step "Clean complete"
        }
        'help' {
            Show-Usage
        }
    }
}
finally {
    Stop-Processes
}
