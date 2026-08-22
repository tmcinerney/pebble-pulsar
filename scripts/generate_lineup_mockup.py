#!/usr/bin/env python3
"""
generate_lineup_mockup.py
Generates a 4-platform comparison lineup showing Aplite, Basalt, Diorite, and Emery.
"""

import os
from PIL import Image, ImageDraw, ImageFont

def create_lineup():
    os.makedirs('screenshots', exist_ok=True)
    width, height = 1200, 540
    img = Image.new('RGB', (width, height), (10, 10, 12))
    draw = ImageDraw.Draw(img)

    # Grid lines
    for x in range(0, width, 40):
        draw.line([(x, 0), (x, height)], fill=(18, 18, 22), width=1)
    for y in range(0, height, 40):
        draw.line([(0, y), (width, y)], fill=(18, 18, 22), width=1)

    try:
        font_title = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 26)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 13)
        font_label = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 13)
        font_sublabel = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 11)
    except Exception:
        font_title = font_sub = font_label = font_sublabel = ImageFont.load_default()

    draw.text((width//2, 32), "PEBBLE HARDWARE LINEUP", fill=(255, 255, 255), font=font_title, anchor="mm")
    draw.text((width//2, 58), "AUTHENTIC PROCEDURAL GaAsP MATRIX RENDERING ON RECTANGULAR DISPLAYS", fill=(220, 50, 50), font=font_sub, anchor="mm")

    platforms = [
        ("APLITE", "Pebble Classic", "screenshots/aplite-time.png"),
        ("BASALT", "Pebble Time / Steel", "screenshots/basalt-time.png"),
        ("DIORITE", "Pebble 2 HR", "screenshots/diorite-time.png"),
        ("EMERY", "Pebble Time 2", "screenshots/emery-time.png"),
    ]

    centers = [165, 435, 705, 995]

    for i, (plat, name, path) in enumerate(platforms):
        cx = centers[i]
        if os.path.exists(path):
            shot = Image.open(path).convert('RGBA')
            scale = 1.35 if plat == "EMERY" else 1.28
            sw = int(shot.width * scale)
            sh = int(shot.height * scale)
            resized = shot.resize((sw, sh), Image.Resampling.LANCZOS)
            
            top_y = 100 + (300 - sh) // 2
            left_x = cx - sw // 2

            radius = 16 if plat == "EMERY" else 12
            draw.rounded_rectangle([left_x - 6, top_y - 6, left_x + sw + 6, top_y + sh + 6], radius=radius, fill=(30, 30, 35), outline=(220, 35, 35) if plat=="EMERY" else (80, 80, 90), width=2 if plat=="EMERY" else 1)
            
            img.paste(resized, (left_x, top_y), resized)

            # Platform label
            draw.text((cx, 440), plat, fill=(255, 255, 255) if plat=="EMERY" else (200, 200, 210), font=font_label, anchor="mm")
            draw.text((cx, 462), name, fill=(180, 50, 50) if plat=="EMERY" else (140, 140, 150), font=font_sublabel, anchor="mm")

    draw.rectangle([0, 0, width - 1, height - 1], outline=(40, 40, 50), width=1)
    
    out_path = 'screenshots/pulsar-all-platforms-mockup.png'
    img.save(out_path, 'PNG')
    print(f"✓ Created 4-Platform Lineup Mockup: {out_path}")

if __name__ == '__main__':
    create_lineup()
