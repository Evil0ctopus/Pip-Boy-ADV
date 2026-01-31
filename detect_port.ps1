# COM Port Auto-Detection and Configuration Script
# Detects M5Stack Cardputer and updates platformio.ini and environment

Write-Host "`n=== M5Stack Cardputer COM Port Detector ===`n" -ForegroundColor Cyan

# Method 1: Try PlatformIO device list (most reliable)
Write-Host "Checking PlatformIO devices..." -ForegroundColor Yellow
$pioDevices = pio device list 2>&1 | Out-String

$detectedPort = $null

# Parse PlatformIO output for COM port
if ($pioDevices -match "(COM\d+)") {
    $detectedPort = $matches[1]
    Write-Host "  ✓ PlatformIO detected: $detectedPort" -ForegroundColor Green
}

# Method 2: Fallback to Windows Device Manager
if (-not $detectedPort) {
    Write-Host "Scanning Windows Device Manager..." -ForegroundColor Yellow
    
    $devices = Get-WmiObject Win32_PnPEntity | Where-Object { 
        $_.Name -match "COM\d+" -and 
        ($_.Name -match "USB" -or $_.Name -match "Serial" -or 
         $_.Name -match "CH340" -or $_.Name -match "CP210" -or
         $_.Name -match "UART")
    }
    
    if ($devices) {
        # Try to find ESP32-S3 specific device
        $esp32Device = $devices | Where-Object { 
            $_.Name -match "CP210" -or $_.Name -match "USB-SERIAL CH340" 
        } | Select-Object -First 1
        
        if ($esp32Device -and $esp32Device.Name -match "(COM\d+)") {
            $detectedPort = $matches[1]
            Write-Host "  ✓ Device Manager detected: $detectedPort" -ForegroundColor Green
            Write-Host "    Device: $($esp32Device.Name)" -ForegroundColor Gray
        }
        elseif ($devices[0].Name -match "(COM\d+)") {
            $detectedPort = $matches[1]
            Write-Host "  ✓ Found USB Serial: $detectedPort" -ForegroundColor Yellow
            Write-Host "    Device: $($devices[0].Name)" -ForegroundColor Gray
        }
    }
}

# Method 3: Last resort - check available ports
if (-not $detectedPort) {
    Write-Host "Checking available COM ports..." -ForegroundColor Yellow
    $availablePorts = [System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object
    
    if ($availablePorts -and $availablePorts.Count -gt 0) {
        $detectedPort = $availablePorts[0]
        Write-Host "  ⚠ Using first available: $detectedPort" -ForegroundColor Yellow
        Write-Host "    (This may not be the correct device)" -ForegroundColor DarkYellow
    }
}

if (-not $detectedPort) {
    Write-Host "`n❌ No COM ports detected!" -ForegroundColor Red
    Write-Host "`nTroubleshooting:" -ForegroundColor Yellow
    Write-Host "  1. Connect M5Stack Cardputer via USB-C cable" -ForegroundColor White
    Write-Host "  2. Ensure cable supports data transfer (not charging-only)" -ForegroundColor White
    Write-Host "  3. Check Device Manager for USB Serial devices" -ForegroundColor White
    Write-Host "  4. Install CP2102 or CH340 drivers if needed" -ForegroundColor White
    Write-Host "  5. Try a different USB port or cable" -ForegroundColor White
    Write-Host "`nAfter connecting, run this script again.`n" -ForegroundColor Cyan
    exit 1
}

Write-Host "`n✓ COM port detected: $detectedPort" -ForegroundColor Green

# Set environment variable for current session
$env:PLATFORMIO_UPLOAD_PORT = $detectedPort
Write-Host "  ✓ Environment variable set: PLATFORMIO_UPLOAD_PORT=$detectedPort" -ForegroundColor Green

# Update platformio.ini
Write-Host "`nUpdating platformio.ini..." -ForegroundColor Cyan

$iniPath = "platformio.ini"
$iniContent = Get-Content $iniPath -Raw

# Check if upload_port already exists and update it
if ($iniContent -match "upload_port\s*=\s*COM\d+") {
    $iniContent = $iniContent -replace "upload_port\s*=\s*COM\d+", "upload_port = $detectedPort"
    Write-Host "  ✓ Updated existing upload_port to: $detectedPort" -ForegroundColor Green
}
elseif ($iniContent -match "; Uncomment and set manually if auto-detection fails: upload_port = COM4") {
    $iniContent = $iniContent -replace "; Uncomment and set manually if auto-detection fails: upload_port = COM4", "upload_port = $detectedPort  ; Auto-detected"
    Write-Host "  ✓ Added upload_port: $detectedPort" -ForegroundColor Green
}
else {
    # Add after upload_speed line
    $iniContent = $iniContent -replace "(upload_speed\s*=\s*\d+)", "`$1`nupload_port = $detectedPort  ; Auto-detected"
    Write-Host "  ✓ Inserted upload_port: $detectedPort" -ForegroundColor Green
}

Set-Content -Path $iniPath -Value $iniContent -NoNewline

Write-Host "`n✓ Configuration complete!" -ForegroundColor Green
Write-Host "`nYou can now:" -ForegroundColor Cyan
Write-Host "  1. Press Ctrl+Shift+B to build, upload, and monitor" -ForegroundColor White
Write-Host "  2. Or run: pio run --target upload" -ForegroundColor White
Write-Host "  3. Or run: pio device monitor" -ForegroundColor White
Write-Host "`n"
