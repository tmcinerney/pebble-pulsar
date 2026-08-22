#!/usr/bin/env python3
"""
generate_lineup_mockup.py
Generates a wide 5-platform comparison mockup showing Emery, Basalt, Chalk, Diorite, and Aplite.
"""

import os
from PIL import Image, ImageDraw, ImageFont

def create_lineup():
    os.makedirs('screenshots', exist_ok=True)
    width, height = 1200, 500
    img = Image.new('RGB', (width, height), (10, 10, 12))
    draw = ImageDraw.Draw(img)

    # Grid lines
    for x in range(0, width, 40):
        draw.line([(x, 0), (x, height)], fill=(18, 18, 22), width=1)
    for y in range(0, height, 40):
        draw.line([(0, y), (width, y)], fill=(18, 18, 22), width=1)

    try:
        font_title = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 28)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 14)
        font_label = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 13)
        font_sublabel = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 11)
    except Exception:
        font_title = font_sub = font_label = font_sublabel = ImageFont.load_default()

    draw.text((width//2, 32), "ALL PEBBLE PLATFORMS SUPPORTED", fill=(255, 255, 255), font=font_title, anchor="mm")
    draw.text((width//2, 60), "NATIVE PROCEDURAL GaAsP MATRIX RENDERING ON ALL HARDWARE", fill=(220, 50, 50), font=font_sub, anchor="mm")

    platforms = [
        ("APLITE", "Pebble Classic", "screenshots/aplite-time.png", False),
        ("BASALT", "Pebble Time", "screenshots/basalt-time.png", False),
        ("EMERY", "Pebble Time 2", "screenshots/emery-time.png", False),
        ("CHALK", "Time Round", "screenshots/chalk-time.png", True),
        ("DIORITE", "Pebble 2 HR", "screenshots/diorite-time.png", False)
    ]

    centers = [150, 360, 600, 840, 1050]

    for i, (plat, name, path, is_round) in enumerate(platforms):
        cx = centers[i]
        if os.path.exists(path):
            shot = Image.open(path).convert('RGBA')
            scale = 1.35 if plat == "EMERY" else (1.2 if is_round else 1.25)
            sw = int(shot.width * scale)
            sh = int(shot.height * scale)
            resized = shot.resize((sw, sh), Image.Resampling.LANCZOS)
            
            top_y = 100 + (260 - sh) // 2
            left_x = cx - sw // 2

            if is_round:
                draw.ellipse([left_x - 6, top_y - 6, left_x + sw + 6, top_y + sh + 6], fill=(30, 30, 35), outline=(180, 25, 25) if plat=="EMERY" else (80, 80, 90), width=2)
            else:
                radius = 16 if plat == "EMERY" else 10
                draw.rounded_rectangle([left_x - 6, top_y - 6, left_x + sw + 6, top_y + sh + 6], radius=radius, fill=(30, 30, 35), outline=(220, 35, 35) if plat=="EMERY" else (80, 80, 90), width=2 if plat=="EMERY" else 1)
            
            img.paste(resized, (left_x, top_y), resized)

            # Platform label
            draw.text((cx, 405), plat, fill=(255, 255, 255) if plat=="EMERY" else (200, 200, 210), font=font_label, anchor="mm")
            draw.text((cx, 425), name, fill=(180, 50, 50) if plat=="EMERY" else (140, 140, 150), font=font_sublabel, anchor="mm")

    draw.rectangle([0, 0, width - 1, height - 1], outline=(40, 40, 50), width=1)
    
    out_path = 'screenshots/pulsar-all-platforms-mockup.png'
    img.save(out_path, 'PNG')
    print(f"✓ Created 5-Platform Mockup: {out_path}")

if __name__ == '__main__':
    create_lineup()
