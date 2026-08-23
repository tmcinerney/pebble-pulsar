#!/usr/bin/env python3
"""
generate_store_assets.py
Generates the Rebble Appstore Banner (720x320) and Store Icon (260x260)
with authentic Pebble Time 2 hardware mockups.
"""

import os
from PIL import Image, ImageDraw, ImageFont
from generate_hardware_mockups import draw_pebble_time2, draw_pebble_diorite

def create_store_assets():
    os.makedirs('screenshots', exist_ok=True)
    
    # -------------------------------------------------------------
    # 1. 720 x 320 Appstore Banner
    # -------------------------------------------------------------
    width, height = 720, 320
    banner = Image.new('RGB', (width, height), (10, 10, 12))
    draw = ImageDraw.Draw(banner)

    # Ambient ruby glow
    for r in range(240, 0, -4):
        alpha = int(16 * (1 - r / 240))
        draw.ellipse([540 - r, height//2 - r, 540 + r, height//2 + r], fill=(alpha * 3, 0, 0))

    # Grid lines
    for x in range(0, width, 36):
        draw.line([(x, 0), (x, height)], fill=(18, 18, 22), width=1)
    for y in range(0, height, 36):
        draw.line([(0, y), (width, y)], fill=(18, 18, 22), width=1)

    # Hardware Watch Renders on Right
    p2_mock = draw_pebble_diorite(scale=0.48, screen_image_path="screenshots/diorite-time.png")
    t2_mock = draw_pebble_time2(scale=0.56, screen_image_path="screenshots/emery-time.png")

    banner.paste(p2_mock, (360, 20), p2_mock)
    banner.paste(t2_mock, (460, -5), t2_mock)

    # Typography & Branding on Left (x=36 to x=350)
    try:
        font_logo = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 38)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 13)
        font_desc = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 13)
        font_badge = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 12)
    except Exception:
        font_logo = font_sub = font_desc = font_badge = ImageFont.load_default()

    # Red Accent bar
    draw.rectangle([34, 42, 38, 138], fill=(220, 20, 20))

    # Main Brand Text
    draw.text((48, 40), "P U L S A R", fill=(255, 255, 255), font=font_logo)
    draw.text((50, 88), "1 9 7 0   T I M E   C O M P U T E R", fill=(220, 50, 50), font=font_sub)
    draw.text((50, 116), "The 1972 Hamilton Digital LED Icon.", fill=(170, 170, 180), font=font_desc)

    # Bullet Highlights
    bullets = [
        "● Authentic 5x7 GaAsP Dot-Matrix LEDs",
        "● 6 Modes: Time, Sec, Date, Steps, Batt, HR",
        "● Reorderable Tap Cycle & Stealth Wake",
        "● Retro Charging Animations & Nightlight"
    ]
    by = 158
    for b in bullets:
        draw.text((50, by), b, fill=(200, 200, 210), font=font_badge)
        by += 28

    # Border
    draw.rectangle([0, 0, width - 1, height - 1], outline=(45, 45, 55), width=1)

    banner_path = 'screenshots/appstore-banner-720x320.png'
    banner.save(banner_path, 'PNG')
    print(f"✓ Created 720x320 Appstore Banner: {banner_path}")

    # -------------------------------------------------------------
    # 2. 260 x 260 Square Store Icon / Screenshot Box
    # -------------------------------------------------------------
    icon_w, icon_h = 260, 260
    icon = Image.new('RGB', (icon_w, icon_h), (12, 12, 14))
    icon_draw = ImageDraw.Draw(icon)
    
    t2_icon = draw_pebble_time2(scale=0.45, screen_image_path="screenshots/emery-time.png")
    cx = (icon_w - t2_icon.width) // 2
    cy = (icon_h - t2_icon.height) // 2
    icon.paste(t2_icon, (cx, cy), t2_icon)
    icon_draw.rectangle([0, 0, icon_w - 1, icon_h - 1], outline=(45, 45, 55), width=1)
    
    icon_path = 'screenshots/store-icon-260x260.png'
    icon.save(icon_path, 'PNG')
    print(f"✓ Created 260x260 Store Icon: {icon_path}")

if __name__ == '__main__':
    create_store_assets()
