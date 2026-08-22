#!/usr/bin/env python3
"""
generate_hero_mockup.py
Generates the flagship Pulsar 1970 Hero Mockup and All-Platforms lineup.
"""

import os
from PIL import Image, ImageDraw, ImageFont, ImageFilter

def create_hero_mockup():
    os.makedirs('screenshots', exist_ok=True)
    width, height = 1200, 675
    hero = Image.new('RGB', (width, height), (8, 8, 10))
    draw = ImageDraw.Draw(hero)

    # 1. Atmospheric Ruby & Charcoal Glow Backdrop
    for r in range(450, 0, -6):
        alpha = int(22 * (1 - r / 450))
        draw.ellipse([780 - r, 337 - r, 780 + r, 337 + r], fill=(alpha * 2, 0, 0))

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
        "● 5 Modes: Time, Sec, Date, Steps, Battery",
        "● Pebble Health 10-Dot Progress Bar",
        "● 6 Vintage Colorways & 6° Italic Slant",
        "● Stealth Push-to-Wake & Hourly Vibe Chimes"
    ]
    by = 315
    for b in bullets:
        draw.text((64, by), b, fill=(210, 210, 220), font=font_list)
        by += 34

    # Compatible Devices footer pill
    draw.rounded_rectangle([60, 525, 520, 595], radius=8, fill=(18, 18, 22), outline=(40, 40, 50), width=1)
    draw.text((76, 538), "COMPATIBLE PEBBLE PLATFORMS:", fill=(200, 50, 50), font=font_badge)
    draw.text((76, 562), "Pebble Time 2 (Emery) • Time / Steel • Round • P2 • Classic", fill=(170, 170, 180), font=font_desc)

    # 3. Right Watches Display Section
    # Centerpiece: Pebble Time 2 (Emery)
    emery_path = 'screenshots/emery-time.png'
    if not os.path.exists(emery_path):
        emery_path = 'screenshots/emery-time-italic.png'
    
    # Background Secondary Watches:
    # 1. Round (Chalk)
    chalk_path = 'screenshots/chalk-time.png'
    if os.path.exists(chalk_path):
        c_img = Image.open(chalk_path).convert('RGBA')
        c_scale = 1.3
        cw, ch = int(c_img.width * c_scale), int(c_img.height * c_scale)
        c_resized = c_img.resize((cw, ch), Image.Resampling.LANCZOS)
        cx, cy = 600, 90
        draw.ellipse([cx - 8, cy - 8, cx + cw + 8, cy + ch + 8], fill=(30, 30, 35), outline=(100, 20, 20), width=2)
        hero.paste(c_resized, (cx, cy), c_resized)

    # 2. Pebble 2 B&W (Diorite Steps)
    diorite_path = 'screenshots/diorite-steps.png'
    if os.path.exists(diorite_path):
        d_img = Image.open(diorite_path).convert('RGBA')
        d_scale = 1.35
        dw, dh = int(d_img.width * d_scale), int(d_img.height * d_scale)
        d_resized = d_img.resize((dw, dh), Image.Resampling.LANCZOS)
        dx, dy = 960, 260
        draw.rounded_rectangle([dx - 8, dy - 8, dx + dw + 8, dy + dh + 8], radius=16, fill=(28, 28, 32), outline=(80, 80, 90), width=2)
        hero.paste(d_resized, (dx, dy), d_resized)

    # 3. Main Centerpiece: Pebble Time 2 (Emery)
    if os.path.exists(emery_path):
        e_img = Image.open(emery_path).convert('RGBA')
        e_scale = 1.95
        ew, eh = int(e_img.width * e_scale), int(e_img.height * e_scale)
        e_resized = e_img.resize((ew, eh), Image.Resampling.LANCZOS)
        ex, ey = 730, 110
        
        # Outer steel case frame & red bevel highlight
        draw.rounded_rectangle([ex - 12, ey - 12, ex + ew + 12, ey + eh + 12], radius=24, fill=(35, 35, 42), outline=(220, 35, 35), width=3)
        hero.paste(e_resized, (ex, ey), e_resized)

    # Border around canvas
    draw.rectangle([0, 0, width - 1, height - 1], outline=(40, 40, 50), width=1)

    hero_path = 'screenshots/pulsar-time2-hero.png'
    hero.save(hero_path, 'PNG')
    print(f"✓ Created 1200x675 Hero Mockup: {hero_path}")

if __name__ == '__main__':
    create_hero_mockup()
