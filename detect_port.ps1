# COM Port Auto-Detection and Configuration Script
# Detects M5Stack Cardputer and updates platformio.ini

Write-Host "`n=== M5Stack Cardputer COM Port Detector ===`n" -ForegroundColor Cyan

# Method 1: Try .NET SerialPort
$dotnetPorts = [System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object
if ($dotnetPorts) {
    Write-Host "Available COM ports (.NET detection):" -ForegroundColor Yellow
    $dotnetPorts | ForEach-Object { Write-Host "  $_" -ForegroundColor White }
}

# Method 2: Try PlatformIO device list
Write-Host "`nChecking PlatformIO devices..." -ForegroundColor Yellow
$pioOutput = pio device list 2>&1 | Out-String

if ($pioOutput -match "COM\d+") {
    Write-Host "PlatformIO detected devices:" -ForegroundColor Green
    Write-Host $pioOutput
} else {
    Write-Host "  No devices detected by PlatformIO" -ForegroundColor DarkYellow
}

# Method 3: Check Windows Device Manager via WMI
Write-Host "`nScanning Windows Device Manager..." -ForegroundColor Yellow
$devices = Get-WmiObject Win32_PnPEntity | Where-Object { 
    $_.Name -match "COM\d+" -and 
    ($_.Name -match "USB" -or $_.Name -match "Serial" -or $_.Name -match "CH340" -or $_.Name -match "CP210")
}

if ($devices) {
    Write-Host "USB Serial devices found:" -ForegroundColor Green
    $devices | ForEach-Object {
        if ($_.Name -match "(COM\d+)") {
            $port = $matches[1]
            Write-Host "  $($_.Name) - $port" -ForegroundColor White
        }
    }
} else {
    Write-Host "  No USB serial devices found" -ForegroundColor DarkYellow
}

# Detect the most likely COM port
$detectedPort = $null

# Check PlatformIO output first (most reliable)
if ($pioOutput -match "(COM\d+)") {
    $detectedPort = $matches[1]
    Write-Host "`n✓ Detected port from PlatformIO: $detectedPort" -ForegroundColor Green
}
# Fallback to WMI detection
elseif ($devices -and $devices[0].Name -match "(COM\d+)") {
    $detectedPort = $matches[1]
    Write-Host "`n✓ Detected port from Device Manager: $detectedPort" -ForegroundColor Green
}
# Last resort: use first available port
elseif ($dotnetPorts -and $dotnetPorts.Count -gt 0) {
    $detectedPort = $dotnetPorts[0]
    Write-Host "`n⚠ Using first available port: $detectedPort" -ForegroundColor Yellow
    Write-Host "  (This may not be the correct device)" -ForegroundColor DarkYellow
}

if (-not $detectedPort) {
    Write-Host "`n❌ No COM ports detected!" -ForegroundColor Red
    Write-Host "`nTroubleshooting:" -ForegroundColor Yellow
    Write-Host "  1. Connect M5Stack Cardputer via USB-C cable" -ForegroundColor White
    Write-Host "  2. Ensure cable supports data transfer (not charging-only)" -ForegroundColor White
    Write-Host "  3. Check Device Manager for USB Serial devices" -ForegroundColor White
    Write-Host "  4. Install CP2102 or CH340 drivers if needed" -ForegroundColor White
    Write-Host "`nAfter connecting, run this script again.`n" -ForegroundColor Cyan
    exit 1
}

# Update platformio.ini
Write-Host "`nUpdating platformio.ini..." -ForegroundColor Cyan

$iniPath = "platformio.ini"
$iniContent = Get-Content $iniPath -Raw

# Check if upload_port already exists
if ($iniContent -match "upload_port\s*=\s*COM\d+") {
    # Replace existing port
    $iniContent = $iniContent -replace "upload_port\s*=\s*COM\d+", "upload_port = $detectedPort"
    Write-Host "  Updated existing upload_port to: $detectedPort" -ForegroundColor Green
}
elseif ($iniContent -match "; Uncomment and set manually if auto-detection fails: upload_port = COM4") {
    # Replace the comment with actual port
    $iniContent = $iniContent -replace "; Uncomment and set manually if auto-detection fails: upload_port = COM4", "upload_port = $detectedPort  ; Auto-detected"
    Write-Host "  Added upload_port: $detectedPort" -ForegroundColor Green
}
else {
    # Add after upload_speed line
    $iniContent = $iniContent -replace "(upload_speed\s*=\s*\d+)", "`$1`nupload_port = $detectedPort  ; Auto-detected"
    Write-Host "  Inserted upload_port: $detectedPort" -ForegroundColor Green
}

Set-Content -Path $iniPath -Value $iniContent -NoNewline

Write-Host "`n✓ Configuration updated!" -ForegroundColor Green
Write-Host "`nYou can now:" -ForegroundColor Cyan
Write-Host "  1. Press Ctrl+Shift+B to build, upload, and monitor" -ForegroundColor White
Write-Host "  2. Or run: pio run --target upload --target monitor" -ForegroundColor White
Write-Host "`n"
