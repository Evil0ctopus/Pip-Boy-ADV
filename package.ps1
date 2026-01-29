# M5 Launcher Firmware Packager - Simple Version
param([string]$Drive = "D")

Write-Host "`n=== Pip-Boy ADV M5 Launcher Packager ===`n" -ForegroundColor Cyan

# Check firmware exists
if (-not (Test-Path ".pio\build\cardputer-adv\firmware.bin")) {
    Write-Host "ERROR: Firmware not built! Run 'pio run' first.`n" -ForegroundColor Red
    exit 1
}

# Get version
$version = "1.0.12"
if (Test-Path "version.txt") {
    $version = (Get-Content "version.txt").Trim()
}

# Setup paths
$sdrive = "${Drive}:"
$dest = Join-Path $sdrive "launcher\downloads\PipBoyADV"
$src = ".pio\build\cardputer-adv"

# Check drive exists
if (-not (Test-Path $sdrive)) {
    Write-Host "ERROR: Drive $sdrive not found!`n" -ForegroundColor Red
    Get-PSDrive -PSProvider FileSystem | Where-Object {$_.Used -gt 0} | ForEach-Object {
        Write-Host "  Available: $($_.Name):" -ForegroundColor Yellow
    }
    Write-Host "`nUsage: .\package.ps1 [DriveLetter]`n" -ForegroundColor White
    exit 1
}

# Create directory
Write-Host "Creating directory: $dest" -ForegroundColor Yellow
New-Item -ItemType Directory -Path $dest -Force | Out-Null

# Copy files
Write-Host "Copying firmware files..." -ForegroundColor Yellow
Copy-Item "$src\firmware.bin" "$dest\firmware.bin" -Force
Copy-Item "$src\bootloader.bin" "$dest\bootloader.bin" -Force
Copy-Item "$src\partitions.bin" "$dest\partitions.bin" -Force
Write-Host "  firmware.bin, bootloader.bin, partitions.bin" -ForegroundColor Green

# Copy icon if exists
if (Test-Path "icon.png") {
    Copy-Item "icon.png" "$dest\icon.png" -Force
    Write-Host "  icon.png" -ForegroundColor Green
}

# Create app.json
$json = @"
{
    "name": "Pip-Boy ADV",
    "version": "$version",
    "author": "Evil0ctopus",
    "description": "Pip-Boy OS with CRT theme and LoRa support",
    "category": "firmware"
}
"@

Set-Content "$dest\app.json" $json -Force
Write-Host "  app.json" -ForegroundColor Green

# Summary
Write-Host "`n=== Package Created Successfully! ===`n" -ForegroundColor Green
Write-Host "Location: $dest" -ForegroundColor White
Write-Host "Version: $version" -ForegroundColor White
Write-Host "`nFiles:" -ForegroundColor Cyan
Get-ChildItem $dest | ForEach-Object {
    $size = [math]::Round($_.Length / 1KB, 1)
    Write-Host "  $($_.Name) - $size KB" -ForegroundColor White
}

Write-Host "`n=== Installation Instructions ===`n" -ForegroundColor Yellow
Write-Host "1. Safely eject SD card" -ForegroundColor White
Write-Host "2. Insert into M5Stack Cardputer" -ForegroundColor White
Write-Host "3. Open M5 Launcher > Install Firmware > PipBoyADV" -ForegroundColor White
Write-Host "4. Wait for installation (~30-60 sec)" -ForegroundColor White
Write-Host "5. Device reboots with Green Pip-Boy UI`n" -ForegroundColor White

Write-Host "Done!`n" -ForegroundColor Green
