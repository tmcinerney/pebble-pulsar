#!/usr/bin/env python3
"""
generate_store_assets.py
Generates the Rebble Appstore Banner (720x320), Store Icon (260x260), and Hero Mockups.
"""

import os
from PIL import Image, ImageDraw, ImageFont

def create_store_assets():
    os.makedirs('screenshots', exist_ok=True)
    
    # -------------------------------------------------------------
    # 1. 720 x 320 Appstore Banner
    # -------------------------------------------------------------
    width, height = 720, 320
    banner = Image.new('RGB', (width, height), (10, 10, 12))
    draw = ImageDraw.Draw(banner)

    # Ambient ruby glow
    for r in range(220, 0, -4):
        alpha = int(14 * (1 - r / 220))
        draw.ellipse([540 - r, height//2 - r, 540 + r, height//2 + r], fill=(alpha * 3, 0, 0))

    # Grid lines
    for x in range(0, width, 36):
        draw.line([(x, 0), (x, height)], fill=(18, 18, 22), width=1)
    for y in range(0, height, 36):
        draw.line([(0, y), (width, y)], fill=(18, 18, 22), width=1)

    # Add Watch Screenshots on the Right
    emery_path = 'screenshots/emery-time.png'
    steps_path = 'screenshots/emery-steps.png'
    
    # Left watch (Steps or Diorite)
    if os.path.exists(steps_path):
        steps_img = Image.open(steps_path).convert('RGBA')
        scale = 0.85
        sw, sh = int(steps_img.width * scale), int(steps_img.height * scale)
        steps_resized = steps_img.resize((sw, steps_img.height * sw // steps_img.width), Image.Resampling.LANCZOS)
        case_x, case_y = 370, 62
        draw.rounded_rectangle([case_x - 5, case_y - 5, case_x + sw + 5, case_y + sh + 5], radius=10, fill=(24, 24, 28), outline=(130, 25, 25), width=1)
        banner.paste(steps_resized, (case_x, case_y), steps_resized)

    # Right primary watch (Emery Time)
    if os.path.exists(emery_path):
        emery_img = Image.open(emery_path).convert('RGBA')
        scale = 1.08
        ew, eh = int(emery_img.width * scale), int(emery_img.height * scale)
        emery_resized = emery_img.resize((ew, eh), Image.Resampling.LANCZOS)
        case_x, case_y = 495, 34
        draw.rounded_rectangle([case_x - 6, case_y - 6, case_x + ew + 6, case_y + eh + 6], radius=12, fill=(32, 32, 36), outline=(220, 30, 30), width=2)
        banner.paste(emery_resized, (case_x, case_y), emery_resized)

    # Typography & Branding on Left (constrained to x=36 to x=355)
    try:
        font_logo = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 40)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 13)
        font_desc = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 13)
        font_badge = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 12)
    except Exception:
        font_logo = font_sub = font_desc = font_badge = ImageFont.load_default()

    # Red Accent bar
    draw.rectangle([34, 42, 38, 138], fill=(220, 20, 20))

    # Main Brand Text
    draw.text((48, 40), "P U L S A R", fill=(255, 255, 255), font=font_logo)
    draw.text((50, 90), "1 9 7 0   T I M E   C O M P U T E R", fill=(220, 50, 50), font=font_sub)
    draw.text((50, 118), "The 1972 Hamilton Digital LED Icon.", fill=(170, 170, 180), font=font_desc)

    # Bullet Highlights
    bullets = [
        "● Authentic 5x7 GaAsP Dot-Matrix LEDs",
        "● 5 Modes: Time, Sec, Date, Steps, Batt",
        "● Pebble Health 10-Dot Progress Bar",
        "● Pebble Time 2, Time, P2 & Classic"
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
    
    if os.path.exists(emery_path):
        emery_img = Image.open(emery_path).convert('RGBA')
        iw, ih = 196, int(196 * emery_img.height / emery_img.width)
        emery_resized = emery_img.resize((iw, ih), Image.Resampling.LANCZOS)
        cx = (icon_w - iw) // 2
        cy = (icon_h - ih) // 2
        
        icon_draw.rounded_rectangle([cx - 4, cy - 4, cx + iw + 4, cy + ih + 4], radius=10, fill=(25, 25, 30), outline=(200, 25, 25), width=2)
        icon.paste(emery_resized, (cx, cy), emery_resized)
    
    icon_path = 'screenshots/store-icon-260x260.png'
    icon.save(icon_path, 'PNG')
    print(f"✓ Created 260x260 Store Icon: {icon_path}")

if __name__ == '__main__':
    create_store_assets()
