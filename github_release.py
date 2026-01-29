#!/usr/bin/env python3
"""
GitHub Release Packaging Script
Creates a versioned ZIP file for GitHub releases with all firmware files
"""

Import("env")
import os
import shutil
import zipfile
from datetime import datetime

def package_for_github(source, target, env):
    """Package firmware for GitHub release"""
    
    print("=" * 60)
    print("GitHub Release Packaging")
    print("=" * 60)
    
    project_dir = env['PROJECT_DIR']
    build_dir = os.path.join(project_dir, '.pio', 'build', 'cardputer-adv')
    releases_dir = os.path.join(project_dir, 'releases')
    
    # Read version
    version_file = os.path.join(project_dir, 'version.txt')
    try:
        with open(version_file, 'r') as f:
            version = f.read().strip()
    except:
        version = '1.0.0'
    
    # Create releases directory
    os.makedirs(releases_dir, exist_ok=True)
    
    # Create timestamped folder name
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    release_name = f"PipBoyADV_v{version}_{timestamp}"
    zip_path = os.path.join(releases_dir, f"{release_name}.zip")
    
    # Files to include
    files_to_package = [
        (os.path.join(build_dir, 'firmware.bin'), 'firmware.bin'),
        (os.path.join(build_dir, 'bootloader.bin'), 'bootloader.bin'),
        (os.path.join(build_dir, 'partitions.bin'), 'partitions.bin'),
        (os.path.join(project_dir, 'icon.png'), 'icon.png'),
    ]
    
    # Create app.json
    app_json_content = f'''{{
  "name": "Pip-Boy ADV",
  "version": "{version}",
  "author": "Evil0ctopus",
  "description": "Pip-Boy OS for M5Stack Cardputer ADV - Full CRT theme with sound effects",
  "type": "firmware",
  "entry": "firmware.bin",
  "icon": "icon.png"
}}'''
    
    # Create temporary app.json
    temp_app_json = os.path.join(build_dir, 'app.json')
    with open(temp_app_json, 'w') as f:
        f.write(app_json_content)
    
    files_to_package.append((temp_app_json, 'app.json'))
    
    # Create README for release
    readme_content = f"""# Pip-Boy ADV Firmware v{version}

## Installation

### Method 1: M5 Launcher (Recommended)
1. Copy all files to SD card folder: `/firmware/PipBoyADV/`
2. Insert SD card into M5Stack Cardputer
3. Open M5 Launcher
4. Navigate to "Install Firmware"
5. Select "Pip-Boy ADV"
6. Wait for installation to complete
7. Device will reboot automatically

### Method 2: Esptool Flash
```bash
esptool.py --chip esp32s3 --port COM_PORT --baud 1500000 \\
  --before default_reset --after hard_reset write_flash \\
  -z --flash_mode dio --flash_freq 80m --flash_size 16MB \\
  0x0 bootloader.bin \\
  0x8000 partitions.bin \\
  0x10000 firmware.bin
```

## Features
- Full Pip-Boy green CRT theme with scanlines and vignette
- Enhanced boot animation with flicker and initialization text
- Sound effects for tabs and interactions (toggle in SYSTEM tab)
- Real-time system stats (battery, CPU, memory, uptime)
- Auto-incrementing version system
- 5 functional tabs: STATUS | DATA | MAP | RADIO | SYSTEM
- LoRa SX1262 support for Radio tab
- No WiFi/network dependencies - instant boot

## Build Info
- Version: {version}
- Build Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
- Platform: ESP32-S3 @ 240MHz
- Framework: Arduino + ESP-IDF
- LVGL: 8.4.0

## File Listing
- firmware.bin - Main application firmware
- bootloader.bin - ESP32-S3 bootloader
- partitions.bin - Partition table
- app.json - M5 Launcher metadata
- icon.png - Launcher icon (64x64 PNG)

## Changelog
See GitHub releases for detailed changelog.
"""
    
    # Create temporary README
    temp_readme = os.path.join(build_dir, 'README.txt')
    with open(temp_readme, 'w') as f:
        f.write(readme_content)
    
    files_to_package.append((temp_readme, 'README.txt'))
    
    # Create ZIP file
    print(f"\nCreating release package: {release_name}.zip")
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for src_path, arc_name in files_to_package:
            if os.path.exists(src_path):
                zipf.write(src_path, arc_name)
                file_size = os.path.getsize(src_path)
                print(f"  ✓ {arc_name} ({file_size:,} bytes)")
            else:
                print(f"  ⚠ {arc_name} (missing - skipped)")
    
    # Get final ZIP size
    zip_size = os.path.getsize(zip_path)
    
    print("\n" + "=" * 60)
    print(f"Release package created successfully!")
    print(f"Location: {zip_path}")
    print(f"Size: {zip_size:,} bytes")
    print("=" * 60)

# Add post-build action
env.AddPostAction("$BUILD_DIR/firmware.bin", package_for_github)
