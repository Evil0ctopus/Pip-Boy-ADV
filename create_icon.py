#!/usr/bin/env python3
"""
Create a Pip-Boy style icon for M5 Launcher
Generates a 64x64 PNG with green CRT style
"""

try:
    from PIL import Image, ImageDraw, ImageFont
    import os

    # Create 64x64 image with black background
    img = Image.new('RGB', (64, 64), color=(0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Pip-Boy green color
    green = (0, 255, 0)
    green_dark = (0, 128, 0)

    # Draw outer border
    draw.rectangle([2, 2, 61, 61], outline=green, width=2)

    # Draw "PIP-BOY" text (simple, without font file)
    # Top text
    draw.text((10, 12), "PIP-", fill=green_dark)
    draw.text((10, 12), "PIP-", fill=green)  # Offset slightly for bold effect
    draw.text((32, 12), "BOY", fill=green_dark)
    draw.text((32, 12), "BOY", fill=green)

    # Draw "ADV" text
    draw.text((20, 35), "ADV", fill=green_dark)
    draw.text((20, 35), "ADV", fill=green)

    # Draw scanline effect
    for y in range(4, 60, 4):
        draw.line([(4, y), (60, y)], fill=(0, 64, 0), width=1)

    # Save icon
    output_path = os.path.join(os.path.dirname(__file__), 'icon.png')
    img.save(output_path)
    print(f"Icon created: {output_path}")

except ImportError:
    print("ERROR: Pillow library not installed")
    print("Install with: pip install Pillow")
    print("Creating placeholder icon.png...")
    
    # Create a minimal 64x64 green square as fallback
    import struct
    
    # Create a minimal PNG (just a green square)
    width = height = 64
    
    def create_minimal_png():
        """Create a minimal PNG file manually"""
        # This is a placeholder - in real deployment, use PIL or provide a pre-made icon
        with open('icon.png', 'wb') as f:
            # PNG signature
            f.write(b'\x89PNG\r\n\x1a\n')
            
            # IHDR chunk
            ihdr = struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)
            f.write(struct.pack('>I', 13))  # Length
            f.write(b'IHDR')
            f.write(ihdr)
            f.write(struct.pack('>I', 0))  # CRC (placeholder)
            
            # Note: This creates an incomplete PNG
            # For production, use PIL or provide a pre-made icon.png
            print("Minimal placeholder created - replace with proper icon using PIL")
    
    # Skip creating invalid PNG - user should install PIL
    print("Please install Pillow library to generate icon")
