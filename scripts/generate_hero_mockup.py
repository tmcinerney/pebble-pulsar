#!/usr/bin/env python3
"""
generate_hero_mockup.py
Generates the flagship Pulsar 1970 Hero Mockup.
"""

import os
from PIL import Image, ImageDraw, ImageFont

def create_hero_mockup():
    os.makedirs('screenshots', exist_ok=True)
    width, height = 1200, 675
    hero = Image.new('RGB', (width, height), (8, 8, 10))
    draw = ImageDraw.Draw(hero)

    # 1. Atmospheric Ruby & Charcoal Glow Backdrop
    for r in range(450, 0, -6):
        alpha = int(22 * (1 - r / 450))
        draw.ellipse([800 - r, 337 - r, 800 + r, 337 + r], fill=(alpha * 2, 0, 0))

    # Space-age vintage grid pattern
    for x in range(0, width, 40):
        draw.line([(x, 0), (x, height)], fill=(16, 16, 20), width=1)
    for y in range(0, height, 40):
        draw.line([(0, y), (width, y)], fill=(16, 16, 20), width=1)

    # 2. Left Typography Section
    try:
        font_badge = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 15)
        font_title = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 64)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 22)
        font_desc = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 17)
        font_list = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 15)
    except Exception:
        font_badge = font_title = font_sub = font_desc = font_list = ImageFont.load_default()

    # Red accent badge
    draw.rectangle([60, 65, 230, 92], fill=(200, 25, 25))
    draw.text((70, 70), "REBBLE APPSTORE", fill=(255, 255, 255), font=font_badge)

    draw.text((60, 115), "P U L S A R", fill=(255, 255, 255), font=font_title)
    draw.text((64, 190), "1 9 7 0   T I M E   C O M P U T E R", fill=(230, 45, 45), font=font_sub)
    draw.text((64, 235), "The world's first commercial digital LED wristwatch,\npainstakingly recreated for the entire Pebble family.", fill=(180, 180, 190), font=font_desc)

    bullets = [
        "● Procedural 5x7 GaAsP Dot-Matrix LEDs",
        "● 6 Modes: Time, Sec, Date, Steps, Batt, HR",
        "● Reorderable Custom Tap Cycle Sequence",
        "● Clock-Synchronized Charging Animations",
        "● Step Overdrive Celebration & Nightlight"
    ]
    by = 315
    for b in bullets:
        draw.text((64, by), b, fill=(210, 210, 220), font=font_list)
        by += 34

    # Compatible Devices footer pill
    draw.rounded_rectangle([60, 525, 520, 595], radius=8, fill=(18, 18, 22), outline=(40, 40, 50), width=1)
    draw.text((76, 538), "COMPATIBLE PEBBLE PLATFORMS:", fill=(200, 50, 50), font=font_badge)
    draw.text((76, 562), "Pebble Time 2 (Emery) • Time / Steel • Pebble 2 • Classic", fill=(170, 170, 180), font=font_desc)

    # 3. Right Watches Display Section
    emery_path = 'screenshots/emery-time.png'
    basalt_path = 'screenshots/basalt-time.png'
    diorite_path = 'screenshots/diorite-time.png'

    # 1. Pebble Time (Basalt) - Left Secondary
    if os.path.exists(basalt_path):
        b_img = Image.open(basalt_path).convert('RGBA')
        b_scale = 1.32
        bw, bh = int(b_img.width * b_scale), int(b_img.height * b_scale)
        b_resized = b_img.resize((bw, bh), Image.Resampling.LANCZOS)
        bx, by_pos = 580, 175
        draw.rounded_rectangle([bx - 8, by_pos - 8, bx + bw + 8, by_pos + bh + 8], radius=14, fill=(26, 26, 30), outline=(100, 25, 25), width=2)
        hero.paste(b_resized, (bx, by_pos), b_resized)

    # 2. Pebble 2 B&W (Diorite) - Right Secondary
    if os.path.exists(diorite_path):
        d_img = Image.open(diorite_path).convert('RGBA')
        d_scale = 1.32
        dw, dh = int(d_img.width * d_scale), int(d_img.height * d_scale)
        d_resized = d_img.resize((dw, dh), Image.Resampling.LANCZOS)
        dx, dy_pos = 965, 220
        draw.rounded_rectangle([dx - 8, dy_pos - 8, dx + dw + 8, dy_pos + dh + 8], radius=14, fill=(28, 28, 32), outline=(80, 80, 90), width=2)
        hero.paste(d_resized, (dx, dy_pos), d_resized)

    # 3. Main Centerpiece: Pebble Time 2 (Emery)
    if os.path.exists(emery_path):
        e_img = Image.open(emery_path).convert('RGBA')
        e_scale = 1.85
        ew, eh = int(e_img.width * e_scale), int(e_img.height * e_scale)
        e_resized = e_img.resize((ew, eh), Image.Resampling.LANCZOS)
        ex, ey_pos = 730, 110
        
        # Outer steel case frame & red bevel highlight
        draw.rounded_rectangle([ex - 10, ey_pos - 10, ex + ew + 10, ey_pos + eh + 10], radius=22, fill=(35, 35, 42), outline=(220, 35, 35), width=3)
        hero.paste(e_resized, (ex, ey_pos), e_resized)

    draw.rectangle([0, 0, width - 1, height - 1], outline=(40, 40, 50), width=1)
    
    out_path = 'screenshots/pulsar-time2-hero.png'
    hero.save(out_path, 'PNG')
    print(f"✓ Created Hero Mockup: {out_path}")

if __name__ == '__main__':
    create_hero_mockup()
