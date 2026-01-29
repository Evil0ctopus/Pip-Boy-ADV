# ==============================================================================
# Pip-Boy ADV - M5 Launcher Firmware Packaging Script
# ==============================================================================
# This script packages the compiled firmware for M5 Launcher installation
# Usage: .\package_m5launcher.ps1 [DriveLetter]
# Example: .\package_m5launcher.ps1 D
# ==============================================================================

param(
    [string]$DriveLetter = "D"  # Default to D: drive, can be overridden
)

Write-Host "`n╔════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║   Pip-Boy ADV M5 Launcher Packager    ║" -ForegroundColor Cyan
Write-Host "╚════════════════════════════════════════╝`n" -ForegroundColor Cyan

# Check if firmware was built
$buildPath = ".pio\build\cardputer-adv"
$firmwareBin = Join-Path $buildPath "firmware.bin"

if (-not (Test-Path $firmwareBin)) {
    Write-Host "ERROR: Firmware not found!" -ForegroundColor Red
    Write-Host "Please run 'pio run' first to build the firmware." -ForegroundColor Yellow
    exit 1
}

Write-Host "✓ Firmware found: $firmwareBin" -ForegroundColor Green

# Get version
$version = "1.0.12"  # Default version
if (Test-Path "version.txt") {
    $version = (Get-Content "version.txt").Trim()
    Write-Host "✓ Version: $version" -ForegroundColor Green
}

# Setup paths
$sdDrive = "${DriveLetter}:"
$otaPath = Join-Path $sdDrive "launcher\downloads\PipBoyADV"

# Check if drive exists
if (-not (Test-Path $sdDrive)) {
    Write-Host "`nERROR: Drive $sdDrive not found!" -ForegroundColor Red
    Write-Host "`nAvailable drives:" -ForegroundColor Yellow
    Get-PSDrive -PSProvider FileSystem | Where-Object {$_.Used -gt 0} | ForEach-Object {
        $sizeGB = [math]::Round(($_.Used + $_.Free) / 1GB, 2)
        Write-Host "  $($_.Name): $sizeGB GB" -ForegroundColor White
    }
    Write-Host "`nUsage: .\package_m5launcher.ps1 [DriveLetter]" -ForegroundColor Yellow
    Write-Host "Example: .\package_m5launcher.ps1 E`n" -ForegroundColor Yellow
    exit 1
}

Write-Host "✓ Target drive: $sdDrive" -ForegroundColor Green

# Create directory structure
Write-Host "`nCreating M5 Launcher directory structure..." -ForegroundColor Yellow
try {
    New-Item -ItemType Directory -Path $otaPath -Force | Out-Null
    Write-Host "✓ Created: $otaPath" -ForegroundColor Green
} catch {
    Write-Host "ERROR: Failed to create directory!" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}

# Copy firmware files
Write-Host "`nCopying firmware files..." -ForegroundColor Yellow

$files = @(
    @{Name="firmware.bin"; Required=$true},
    @{Name="bootloader.bin"; Required=$true},
    @{Name="partitions.bin"; Required=$true}
)

foreach ($file in $files) {
    $srcPath = Join-Path $buildPath $file.Name
    $dstPath = Join-Path $otaPath $file.Name
    
    if (Test-Path $srcPath) {
        Copy-Item -Path $srcPath -Destination $dstPath -Force
        $size = (Get-Item $srcPath).Length
        Write-Host "  ✓ $($file.Name) ($([math]::Round($size/1KB, 1)) KB)" -ForegroundColor Green
    } elseif ($file.Required) {
        Write-Host "  ✗ $($file.Name) MISSING!" -ForegroundColor Red
        exit 1
    } else {
        Write-Host "  ⚠ $($file.Name) not found (optional)" -ForegroundColor Yellow
    }
}

# Copy or create icon.png
Write-Host "`nProcessing icon..." -ForegroundColor Yellow
$iconSrc = "icon.png"
$iconDst = Join-Path $otaPath "icon.png"

if (Test-Path $iconSrc) {
    Copy-Item -Path $iconSrc -Destination $iconDst -Force
    Write-Host "  ✓ icon.png copied" -ForegroundColor Green
} else {
    # Create a minimal PNG placeholder if icon doesn't exist
    Write-Host "  ⚠ icon.png not found, using placeholder" -ForegroundColor Yellow
    # For now, just copy from source if it exists there
}

# Create app.json manifest
Write-Host "`nCreating M5 Launcher manifest..." -ForegroundColor Yellow

$manifest = @{
    name = "Pip-Boy ADV"
    version = $version
    author = "Evil0ctopus"
    description = "Pip-Boy OS with CRT theme, LoRa support, and enhanced features"
    category = "firmware"
    homepage = "https://github.com/Evil0ctal/Pip-Boy-ADV"
}

$manifestJson = $manifest | ConvertTo-Json -Depth 10
$manifestPath = Join-Path $otaPath "app.json"
Set-Content -Path $manifestPath -Value $manifestJson -Force

Write-Host "  ✓ app.json created" -ForegroundColor Green

# Verify all files
Write-Host "`nVerifying package..." -ForegroundColor Yellow

$requiredFiles = @("firmware.bin", "bootloader.bin", "partitions.bin", "app.json")
$allPresent = $true

foreach ($file in $requiredFiles) {
    $path = Join-Path $otaPath $file
    if (Test-Path $path) {
        Write-Host "  ✓ $file" -ForegroundColor Green
    } else {
        Write-Host "  ✗ $file MISSING!" -ForegroundColor Red
        $allPresent = $false
    }
}

if (-not $allPresent) {
    Write-Host "`nERROR: Package incomplete!" -ForegroundColor Red
    exit 1
}

# Display summary
Write-Host "`n╔════════════════════════════════════════╗" -ForegroundColor Green
Write-Host "║   M5 Launcher Package Created!         ║" -ForegroundColor Green
Write-Host "╚════════════════════════════════════════╝`n" -ForegroundColor Green

Write-Host "Package Details:" -ForegroundColor Cyan
Write-Host "  Location: $otaPath" -ForegroundColor White
Write-Host "  Version: $version" -ForegroundColor White
Write-Host "  App Name: Pip-Boy ADV" -ForegroundColor White

Write-Host "`nM5 Launcher Structure:" -ForegroundColor Cyan
Get-ChildItem -Path $otaPath | ForEach-Object {
    $size = if (-not $_.PSIsContainer) { " ($([math]::Round($_.Length/1KB, 1)) KB)" } else { "" }
    Write-Host "  $($_.Name)$size" -ForegroundColor White
}

Write-Host "`n╔════════════════════════════════════════╗" -ForegroundColor Yellow
Write-Host "║          Installation Steps            ║" -ForegroundColor Yellow
Write-Host "╚════════════════════════════════════════╝`n" -ForegroundColor Yellow

Write-Host "1. Safely eject the SD card from your computer" -ForegroundColor White
Write-Host "2. Insert SD card into M5Stack Cardputer" -ForegroundColor White
Write-Host "3. Power on the device" -ForegroundColor White
Write-Host "4. Open M5 Launcher application" -ForegroundColor White
Write-Host "5. Navigate to: Install Firmware" -ForegroundColor White
Write-Host "6. Select: PipBoyADV" -ForegroundColor White
Write-Host "7. Confirm installation" -ForegroundColor White
Write-Host "8. Wait for flashing (~30-60 seconds)" -ForegroundColor White
Write-Host "9. Device will reboot automatically`n" -ForegroundColor White

Write-Host "Expected Result: Green Pip-Boy UI displays on screen`n" -ForegroundColor Cyan

Write-Host "For debugging: Connect USB and monitor serial output" -ForegroundColor Yellow
Write-Host "Command: pio device monitor --baud 115200`n" -ForegroundColor Yellow

Write-Host "✓ Packaging complete!`n" -ForegroundColor Green
