// ============================================================================
// VAULT BOY ANIMATION ASSETS SETUP GUIDE
// ============================================================================
//
// To use the proper Vault Boy walking animation, you need to add PNG frames
// from the original Pip-Boy Weather Clock repository.
//
// Source: https://github.com/nishad2m8/Pip-Boy-Weather-Clock
// Asset Directory: /data/assets/Images/walking/
//
// ============================================================================
// SETUP INSTRUCTIONS
// ============================================================================
//
// 1. CLONE THE PIP-BOY WEATHER CLOCK REPO:
//    git clone https://github.com/nishad2m8/Pip-Boy-Weather-Clock.git
//
// 2. COPY THE WALKING ANIMATION FRAMES:
//    From: Pip-Boy-Weather-Clock/data/assets/Images/walking/
//    To:   Pip-Boy-ADV/data/assets/Images/walking/
//
//    Files needed:
//    - walking_00.png through walking_20.png (21 frames total)
//    - Each frame is 64x64 pixels with transparency
//    - Format: PNG with alpha channel
//
// 3. STRUCTURE YOUR DATA FOLDER:
//    Pip-Boy-ADV/data/
//    ├── assets/
//    │   └── Images/
//    │       └── walking/
//    │           ├── walking_00.png
//    │           ├── walking_01.png
//    │           ├── walking_02.png
//    │           └── ... (up to walking_20.png)
//    ├── config.txt (existing)
//    └── themes/ (existing)
//
// 4. BUILD AND UPLOAD:
//    pio run --target upload      (firmware)
//    pio run --target uploadfs    (file system / data folder)
//
// ============================================================================
// ANIMATION SPECIFICATIONS
// ============================================================================
//
// Frame Count:     21 frames (walking_00.png through walking_20.png)
// Frame Size:      64×64 pixels
// Frame Rate:      10 FPS (100ms per frame)
// Total Duration:  2100ms per complete cycle
// Loop:            Infinite
// Format:          PNG with alpha transparency
// Color Palette:   24-bit RGB or RGBA
//
// ============================================================================
// FALLBACK BEHAVIOR
// ============================================================================
//
// If the animation assets are not found:
// - The animation container will still initialize
// - A placeholder message will display: "VAULT BOY\nWALKING\nANIMATION"
// - The UI will remain functional
// - Check serial monitor for error messages
//
// To verify assets are found:
// Serial output will show:
//   ✓ Vault Boy assets found on SD card
//   ✓ Vault Boy animation ready and playing
//
// ============================================================================
// LVGL IMAGE CONFIGURATION
// ============================================================================
//
// For LVGL to properly decode PNG files, ensure these are enabled in lv_conf.h:
// - LV_IMG_CF_TRUE_COLOR
// - LV_IMG_CF_ALPHA_1BIT or LV_IMG_CF_ALPHA_8BIT
// - LV_USE_PNG (if available)
//
// The animation uses lv_img_set_src() to load PNG files directly from the
// file system, which is more RAM-efficient than pre-converting to C arrays.
//
// ============================================================================
// TROUBLESHOOTING
// ============================================================================
//
// Problem: Animation not showing
// Solution: Check serial monitor for "Vault Boy assets not found" message
//           Verify PNG files are in /assets/Images/walking/ directory
//           Ensure filesystem was uploaded with "pio run --target uploadfs"
//
// Problem: Stuttering or slow animation
// Solution: Preload frames using vault_boy_preload_frames()
//           Reduce other UI updates during animation
//           Check if SD card speed is adequate (class 10+ recommended)
//
// Problem: File not found errors
// Solution: Check file paths match exactly: walking_%02d.png
//           Verify PNG file names are lowercase: walking_00.png (not walking_0.png)
//           Check SD card is formatted correctly (FAT32)
//
// ============================================================================
// COLOR REFERENCE (from Pip-Boy Weather Clock)
// ============================================================================
//
// Primary Colors:
// - Phosphor Green:  #00FF33 (LVGL: lv_color_make(0x00, 0xFF, 0x33))
// - Bright Glow:     #33FF66 (LVGL: lv_color_make(0x33, 0xFF, 0x66))
// - Medium:          #00CC22 (LVGL: lv_color_make(0x00, 0xCC, 0x22))
// - Dark:            #008811 (LVGL: lv_color_make(0x00, 0x88, 0x11))
//
// Background Colors:
// - CRT Black:       #001100 (LVGL: lv_color_make(0x00, 0x11, 0x00))
// - CRT Dark:        #002200 (LVGL: lv_color_make(0x00, 0x22, 0x00))
// - Pure Black:      #000000 (LVGL: lv_color_make(0x00, 0x00, 0x00))
//
// ============================================================================
