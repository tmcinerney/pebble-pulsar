#!/usr/bin/env python3
"""
generate_hardware_mockups.py

Procedurally generates authentic, studio-quality hardware device mockups for:
1. Pebble Time 2 (Emery - 200x228 Color) - Brushed Stainless Steel 316L, ultra-slim bezel
2. Pebble Time Steel (Basalt - 144x168 Color) - Gunmetal Steel, classic black glass bezel
3. Pebble 2 HR (Diorite - 144x168 Monochrome) - Matte Sport Polycarbonate
4. Pebble Classic (Aplite - 144x168 Monochrome) - Retro Glossy Polycarbonate

Composites exact pixel-perfect emulator screenshots into the hardware screens.
"""

import os
import math
from PIL import Image, ImageDraw, ImageFont, ImageFilter

def create_strap(draw, cx, cy, case_h, width=160, strap_color=(24, 24, 28)):
    """Draws realistic silicone/leather watchband straps at top and bottom with subtle shadows."""
    top_strap_h = 100
    bottom_strap_h = 100
    
    # Top strap with rounded top
    t_top = cy - case_h // 2 - top_strap_h
    t_bottom = cy - case_h // 2 + 10
    draw.rounded_rectangle([cx - width//2, t_top, cx + width//2, t_bottom], radius=12, fill=strap_color)
    # Subtle horizontal strap ribbing/texture
    for y in range(t_top + 16, t_bottom - 12, 14):
        draw.line([(cx - width//2 + 8, y), (cx + width//2 - 8, y)], fill=(strap_color[0]-6, strap_color[1]-6, strap_color[2]-6), width=2)

    # Bottom strap with rounded bottom
    b_top = cy + case_h // 2 - 10
    b_bottom = cy + case_h // 2 + bottom_strap_h
    draw.rounded_rectangle([cx - width//2, b_top, cx + width//2, b_bottom], radius=12, fill=strap_color)
    for y in range(b_top + 16, b_bottom - 12, 14):
        draw.line([(cx - width//2 + 8, y), (cx + width//2 - 8, y)], fill=(strap_color[0]-6, strap_color[1]-6, strap_color[2]-6), width=2)


def draw_pebble_time2(scale=1.0, screen_image_path="screenshots/emery-time.png"):
    """
    Renders an authentic Pebble Time 2 (Emery).
    - 316L Brushed Stainless Steel chassis with chamfered bevels
    - Slim, uniform outer bezel
    - 200x228 color memory LCD screen (53% larger screen area)
    - 3 textured right buttons, 1 back left button
    """
    canvas_w = int(480 * scale)
    canvas_h = int(600 * scale)
    img = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = canvas_w // 2
    cy = canvas_h // 2

    case_w = int(280 * scale)
    case_h = int(330 * scale)
    corner_r = int(32 * scale)

    # 1. Watch Strap
    strap_w = int(176 * scale)
    create_strap(draw, cx, cy, case_h, width=strap_w, strap_color=(20, 20, 24))

    # 2. Case Drop Shadow
    shadow = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    s_draw = ImageDraw.Draw(shadow)
    s_draw.rounded_rectangle([cx - case_w//2, cy - case_h//2 + 6, cx + case_w//2, cy + case_h//2 + 10], radius=corner_r, fill=(0, 0, 0, 160))
    shadow = shadow.filter(ImageFilter.GaussianBlur(int(16 * scale)))
    img.paste(shadow, (0, 0), shadow)

    # 3. Hardware Buttons (Textured 316L steel)
    btn_w = int(8 * scale)
    # Left back button
    btn_h_left = int(48 * scale)
    draw.rounded_rectangle([cx - case_w//2 - btn_w + 1, cy - btn_h_left//2, cx - case_w//2 + 4, cy + btn_h_left//2], radius=int(3*scale), fill=(75, 78, 86), outline=(135, 140, 150), width=1)
    
    # Right 3 buttons (Up, Select, Down)
    btn_h_right = int(38 * scale)
    btn_spacing = int(48 * scale)
    for offset in [-btn_spacing, 0, btn_spacing]:
        draw.rounded_rectangle([cx + case_w//2 - 4, cy + offset - btn_h_right//2, cx + case_w//2 + btn_w - 1, cy + offset + btn_h_right//2], radius=int(3*scale), fill=(75, 78, 86), outline=(135, 140, 150), width=1)
        for ty in range(cy + offset - btn_h_right//2 + 4, cy + offset + btn_h_right//2 - 2, 4):
            draw.line([(cx + case_w//2, ty), (cx + case_w//2 + btn_w - 4, ty)], fill=(45, 48, 54), width=1)

    # 4. Outer 316L Brushed Stainless Steel Body
    # Outer Bevel Highlight (Light reflection on top edge, shadow on bottom)
    draw.rounded_rectangle([cx - case_w//2, cy - case_h//2, cx + case_w//2, cy + case_h//2], radius=corner_r, fill=(60, 62, 68), outline=(170, 175, 188), width=int(2*scale))
    
    # Brushed Steel Inner Faceplate
    inset1 = int(3 * scale)
    draw.rounded_rectangle([cx - case_w//2 + inset1, cy - case_h//2 + inset1, cx + case_w//2 - inset1, cy + case_h//2 - inset1], radius=corner_r - int(2*scale), fill=(42, 44, 48), outline=(95, 100, 112), width=1)

    # 5. Inner Dark Glass Bezel (Slim uniform 14px bezel!)
    inset2 = int(14 * scale)
    bezel_w = case_w - (inset2 * 2)
    bezel_h = case_h - (inset2 * 2)
    bezel_r = int(18 * scale)
    draw.rounded_rectangle([cx - bezel_w//2, cy - bezel_h//2, cx + bezel_w//2, cy + bezel_h//2], radius=bezel_r, fill=(12, 12, 14), outline=(28, 28, 32), width=1)

    # 6. Active Screen Window (200x228 aspect ratio = 1 : 1.14)
    screen_w = int(220 * scale)
    screen_h = int(250 * scale)
    sx = cx - screen_w // 2
    sy = cy - screen_h // 2

    # Screen Well Bevel / Drop shadow
    draw.rectangle([sx - 1, sy - 1, sx + screen_w, sy + screen_h], outline=(28, 28, 32), width=1)

    # Paste Active Watchface Screenshot
    if os.path.exists(screen_image_path):
        screen = Image.open(screen_image_path).convert('RGBA')
        screen_resized = screen.resize((screen_w, screen_h), Image.Resampling.LANCZOS)
        img.paste(screen_resized, (sx, sy), screen_resized)
    else:
        draw.rectangle([sx, sy, sx + screen_w, sy + screen_h], fill=(0, 0, 0))

    # 7. Mineral Crystal Specular Sheen (Subtle anti-reflective diagonal glint)
    glare = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    g_draw = ImageDraw.Draw(glare)
    g_draw.polygon([
        (cx - bezel_w//2 + 4, cy - bezel_h//2 + 4),
        (cx - bezel_w//2 + int(bezel_w * 0.72), cy - bezel_h//2 + 4),
        (cx - bezel_w//2 + 4, cy - bezel_h//2 + int(bezel_h * 0.65))
    ], fill=(255, 255, 255, 14))
    img.paste(glare, (0, 0), glare)

    return img


def draw_pebble_basalt(scale=1.0, screen_image_path="screenshots/basalt-time.png"):
    """
    Renders an authentic Pebble Time / Time Steel (Basalt).
    - Curved gunmetal steel outer bezel
    - Black glass inner border with 144x168 screen centered inside
    """
    canvas_w = int(480 * scale)
    canvas_h = int(600 * scale)
    img = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = canvas_w // 2
    cy = canvas_h // 2

    case_w = int(270 * scale)
    case_h = int(315 * scale)
    corner_r = int(32 * scale)

    # 1. Watch Strap
    strap_w = int(168 * scale)
    create_strap(draw, cx, cy, case_h, width=strap_w, strap_color=(32, 28, 26))

    # 2. Case Drop Shadow
    shadow = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    s_draw = ImageDraw.Draw(shadow)
    s_draw.rounded_rectangle([cx - case_w//2, cy - case_h//2 + 6, cx + case_w//2, cy + case_h//2 + 10], radius=corner_r, fill=(0, 0, 0, 160))
    shadow = shadow.filter(ImageFilter.GaussianBlur(int(16 * scale)))
    img.paste(shadow, (0, 0), shadow)

    # 3. Hardware Buttons
    btn_w = int(7 * scale)
    btn_h_left = int(44 * scale)
    draw.rounded_rectangle([cx - case_w//2 - btn_w + 1, cy - btn_h_left//2, cx - case_w//2 + 4, cy + btn_h_left//2], radius=int(3*scale), fill=(55, 55, 60), outline=(105, 105, 115), width=1)
    
    btn_h_right = int(34 * scale)
    btn_spacing = int(44 * scale)
    for offset in [-btn_spacing, 0, btn_spacing]:
        draw.rounded_rectangle([cx + case_w//2 - 4, cy + offset - btn_h_right//2, cx + case_w//2 + btn_w - 1, cy + offset + btn_h_right//2], radius=int(3*scale), fill=(55, 55, 60), outline=(105, 105, 115), width=1)

    # 4. Gunmetal Outer Bezel Frame
    draw.rounded_rectangle([cx - case_w//2, cy - case_h//2, cx + case_w//2, cy + case_h//2], radius=corner_r, fill=(48, 50, 55), outline=(120, 125, 138), width=int(2*scale))

    # Black Glass Bezel Area (Characteristic wide bezel of Pebble Time Steel)
    glass_w = int(220 * scale)
    glass_h = int(255 * scale)
    draw.rounded_rectangle([cx - glass_w//2, cy - glass_h//2, cx + glass_w//2, cy + glass_h//2], radius=int(16*scale), fill=(10, 10, 12), outline=(26, 26, 30), width=1)

    # 5. Centered 144x168 Screen
    screen_w = int(148 * scale * 1.05)
    screen_h = int(172 * scale * 1.05)
    sx = cx - screen_w // 2
    sy = cy - screen_h // 2

    draw.rectangle([sx - 1, sy - 1, sx + screen_w, sy + screen_h], outline=(22, 22, 26), width=1)

    if os.path.exists(screen_image_path):
        screen = Image.open(screen_image_path).convert('RGBA')
        screen_resized = screen.resize((screen_w, screen_h), Image.Resampling.LANCZOS)
        img.paste(screen_resized, (sx, sy), screen_resized)

    # Specular Sheen
    glare = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    g_draw = ImageDraw.Draw(glare)
    g_draw.polygon([
        (cx - glass_w//2 + 4, cy - glass_h//2 + 4),
        (cx - glass_w//2 + int(glass_w * 0.7), cy - glass_h//2 + 4),
        (cx - glass_w//2 + 4, cy - glass_h//2 + int(glass_h * 0.6))
    ], fill=(255, 255, 255, 14))
    img.paste(glare, (0, 0), glare)

    return img


def draw_pebble_diorite(scale=1.0, screen_image_path="screenshots/diorite-time.png"):
    """
    Renders an authentic Pebble 2 HR (Diorite).
    - Matte black/charcoal sport polycarbonate body
    - Integrated silicone side button grips
    - Flat glass face with high-contrast 1-bit screen
    """
    canvas_w = int(480 * scale)
    canvas_h = int(600 * scale)
    img = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = canvas_w // 2
    cy = canvas_h // 2

    case_w = int(260 * scale)
    case_h = int(310 * scale)
    corner_r = int(24 * scale)

    # 1. Watch Strap with perforated sports pattern
    strap_w = int(160 * scale)
    create_strap(draw, cx, cy, case_h, width=strap_w, strap_color=(25, 26, 28))

    # 2. Case Drop Shadow
    shadow = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    s_draw = ImageDraw.Draw(shadow)
    s_draw.rounded_rectangle([cx - case_w//2, cy - case_h//2 + 6, cx + case_w//2, cy + case_h//2 + 10], radius=corner_r, fill=(0, 0, 0, 160))
    shadow = shadow.filter(ImageFilter.GaussianBlur(int(16 * scale)))
    img.paste(shadow, (0, 0), shadow)

    # 3. Silicone Integrated Buttons
    btn_w = int(6 * scale)
    draw.rounded_rectangle([cx - case_w//2 - btn_w + 1, cy - int(24*scale), cx - case_w//2 + 2, cy + int(24*scale)], radius=int(2*scale), fill=(35, 36, 40))
    draw.rounded_rectangle([cx + case_w//2 - 2, cy - int(56*scale), cx + case_w//2 + btn_w - 1, cy + int(56*scale)], radius=int(2*scale), fill=(35, 36, 40))

    # 4. Matte Polycarbonate Body
    draw.rounded_rectangle([cx - case_w//2, cy - case_h//2, cx + case_w//2, cy + case_h//2], radius=corner_r, fill=(28, 29, 32), outline=(58, 60, 66), width=int(2*scale))

    # Glass Window
    glass_w = int(210 * scale)
    glass_h = int(250 * scale)
    draw.rounded_rectangle([cx - glass_w//2, cy - glass_h//2, cx + glass_w//2, cy + glass_h//2], radius=int(12*scale), fill=(10, 10, 12), outline=(32, 33, 38), width=1)

    # 5. Screen Window (144x168 1-bit)
    screen_w = int(148 * scale * 1.05)
    screen_h = int(172 * scale * 1.05)
    sx = cx - screen_w // 2
    sy = cy - screen_h // 2

    draw.rectangle([sx - 1, sy - 1, sx + screen_w, sy + screen_h], outline=(25, 25, 28), width=1)

    if os.path.exists(screen_image_path):
        screen = Image.open(screen_image_path).convert('RGBA')
        screen_resized = screen.resize((screen_w, screen_h), Image.Resampling.LANCZOS)
        img.paste(screen_resized, (sx, sy), screen_resized)

    # Specular Sheen
    glare = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    g_draw = ImageDraw.Draw(glare)
    g_draw.polygon([
        (cx - glass_w//2 + 4, cy - glass_h//2 + 4),
        (cx - glass_w//2 + int(glass_w * 0.7), cy - glass_h//2 + 4),
        (cx - glass_w//2 + 4, cy - glass_h//2 + int(glass_h * 0.6))
    ], fill=(255, 255, 255, 12))
    img.paste(glare, (0, 0), glare)

    return img


def draw_pebble_aplite(scale=1.0, screen_image_path="screenshots/aplite-time.png"):
    """
    Renders an authentic Pebble Classic (Aplite).
    - Glossy rectangular chassis with curved top/bottom
    - Classic white 'pebble' wordmark at bottom
    - 144x168 monochrome screen
    """
    canvas_w = int(480 * scale)
    canvas_h = int(600 * scale)
    img = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    cx = canvas_w // 2
    cy = canvas_h // 2

    case_w = int(250 * scale)
    case_h = int(335 * scale)
    corner_r = int(22 * scale)

    # 1. Watch Strap
    strap_w = int(156 * scale)
    create_strap(draw, cx, cy, case_h, width=strap_w, strap_color=(22, 22, 25))

    # 2. Case Drop Shadow
    shadow = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    s_draw = ImageDraw.Draw(shadow)
    s_draw.rounded_rectangle([cx - case_w//2, cy - case_h//2 + 6, cx + case_w//2, cy + case_h//2 + 10], radius=corner_r, fill=(0, 0, 0, 160))
    shadow = shadow.filter(ImageFilter.GaussianBlur(int(16 * scale)))
    img.paste(shadow, (0, 0), shadow)

    # 3. Hardware Buttons
    btn_w = int(7 * scale)
    btn_h = int(30 * scale)
    draw.rounded_rectangle([cx - case_w//2 - btn_w + 1, cy - btn_h//2, cx - case_w//2 + 2, cy + btn_h//2], radius=int(3*scale), fill=(42, 42, 48), outline=(75, 75, 82), width=1)
    for offset in [-int(48*scale), 0, int(48*scale)]:
        draw.rounded_rectangle([cx + case_w//2 - 2, cy + offset - btn_h//2, cx + case_w//2 + btn_w - 1, cy + offset + btn_h//2], radius=int(3*scale), fill=(42, 42, 48), outline=(75, 75, 82), width=1)

    # 4. Glossy Black Polycarbonate Chassis
    draw.rounded_rectangle([cx - case_w//2, cy - case_h//2, cx + case_w//2, cy + case_h//2], radius=corner_r, fill=(18, 18, 22), outline=(52, 54, 62), width=int(2*scale))

    # 5. Screen Window (144x168)
    screen_w = int(148 * scale * 1.05)
    screen_h = int(172 * scale * 1.05)
    sx = cx - screen_w // 2
    sy = cy - screen_h // 2 - int(12 * scale)

    draw.rectangle([sx - 1, sy - 1, sx + screen_w, sy + screen_h], outline=(30, 30, 35), width=1)

    if os.path.exists(screen_image_path):
        screen = Image.open(screen_image_path).convert('RGBA')
        screen_resized = screen.resize((screen_w, screen_h), Image.Resampling.LANCZOS)
        img.paste(screen_resized, (sx, sy), screen_resized)

    # Classic 'pebble' wordmark below screen
    try:
        font_pebble = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", int(12 * scale))
    except Exception:
        font_pebble = ImageFont.load_default()
    draw.text((cx, cy + int(102 * scale)), "pebble", fill=(125, 130, 140), font=font_pebble, anchor="mm")

    # Specular Sheen
    glare = Image.new('RGBA', (canvas_w, canvas_h), (0, 0, 0, 0))
    g_draw = ImageDraw.Draw(glare)
    g_draw.polygon([
        (cx - case_w//2 + 10, cy - case_h//2 + 10),
        (cx - case_w//2 + int(case_w * 0.7), cy - case_h//2 + 10),
        (cx - case_w//2 + 10, cy - case_h//2 + int(case_h * 0.6))
    ], fill=(255, 255, 255, 12))
    img.paste(glare, (0, 0), glare)

    return img


def generate_all():
    os.makedirs('screenshots/mockups', exist_ok=True)
    os.makedirs('screenshots/renders', exist_ok=True)

    print("Generating individual hardware device mockups...")
    t2 = draw_pebble_time2(scale=1.0, screen_image_path="screenshots/emery-time.png")
    t2.save("screenshots/mockups/pebble-time2-device.png", "PNG")
    t2.save("screenshots/renders/pebble-time2-3d-front.png", "PNG")
    print("✓ Created screenshots/mockups/pebble-time2-device.png and screenshots/renders/pebble-time2-3d-front.png")

    basalt = draw_pebble_basalt(scale=1.0, screen_image_path="screenshots/basalt-time.png")
    basalt.save("screenshots/mockups/pebble-basalt-device.png", "PNG")
    print("✓ Created screenshots/mockups/pebble-basalt-device.png")

    diorite = draw_pebble_diorite(scale=1.0, screen_image_path="screenshots/diorite-time.png")
    diorite.save("screenshots/mockups/pebble-diorite-device.png", "PNG")
    print("✓ Created screenshots/mockups/pebble-diorite-device.png")

    aplite = draw_pebble_aplite(scale=1.0, screen_image_path="screenshots/aplite-time.png")
    aplite.save("screenshots/mockups/pebble-aplite-device.png", "PNG")
    print("✓ Created screenshots/mockups/pebble-aplite-device.png")

    # 1. Flagship Hero Banner (1200 x 675)
    hero_w, hero_h = 1200, 675
    hero = Image.new('RGB', (hero_w, hero_h), (8, 8, 10))
    h_draw = ImageDraw.Draw(hero)

    # Ambient ruby glow
    for r in range(480, 0, -6):
        alpha = int(24 * (1 - r / 480))
        h_draw.ellipse([880 - r, 337 - r, 880 + r, 337 + r], fill=(alpha * 2, 0, 0))

    # Grid
    for x in range(0, hero_w, 40):
        h_draw.line([(x, 0), (x, hero_h)], fill=(16, 16, 20), width=1)
    for y in range(0, hero_h, 40):
        h_draw.line([(0, y), (hero_w, y)], fill=(16, 16, 20), width=1)

    try:
        font_badge = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 15)
        font_title = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 62)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 22)
        font_desc = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 17)
        font_list = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 15)
    except Exception:
        font_badge = font_title = font_sub = font_desc = font_list = ImageFont.load_default()

    h_draw.rectangle([60, 65, 230, 92], fill=(200, 25, 25))
    h_draw.text((70, 70), "REBBLE APPSTORE", fill=(255, 255, 255), font=font_badge)
    h_draw.text((60, 115), "P U L S A R", fill=(255, 255, 255), font=font_title)
    h_draw.text((64, 190), "1 9 7 0   T I M E   C O M P U T E R", fill=(230, 45, 45), font=font_sub)
    h_draw.text((64, 235), "The world's first commercial digital LED wristwatch,\npainstakingly recreated for the entire Pebble family.", fill=(180, 180, 190), font=font_desc)

    bullets = [
        "● Procedural 5x7 GaAsP Dot-Matrix LEDs",
        "● 5 Modes: Time, Sec, Date, Steps, Battery",
        "● Pebble Health 10-Dot Progress Bar",
        "● 6 Vintage Colorways & 6° Italic Slant",
        "● Stealth Push-to-Wake & Hourly Vibe Chimes"
    ]
    by = 315
    for b in bullets:
        h_draw.text((64, by), b, fill=(210, 210, 220), font=font_list)
        by += 34

    h_draw.rounded_rectangle([60, 525, 520, 595], radius=8, fill=(18, 18, 22), outline=(40, 40, 50), width=1)
    h_draw.text((76, 538), "COMPATIBLE PEBBLE PLATFORMS:", fill=(200, 50, 50), font=font_badge)
    h_draw.text((76, 562), "Pebble Time 2 (Emery) • Time / Steel • Pebble 2 • Classic", fill=(170, 170, 180), font=font_desc)

    # Hero Hardware Composition
    t2_hero = draw_pebble_time2(scale=0.95, screen_image_path="screenshots/emery-time.png")
    basalt_hero = draw_pebble_basalt(scale=0.78, screen_image_path="screenshots/basalt-time.png")
    diorite_hero = draw_pebble_diorite(scale=0.78, screen_image_path="screenshots/diorite-time.png")

    hero.paste(basalt_hero, (550, 90), basalt_hero)
    hero.paste(diorite_hero, (880, 120), diorite_hero)
    hero.paste(t2_hero, (680, 40), t2_hero)

    h_draw.rectangle([0, 0, hero_w - 1, hero_h - 1], outline=(40, 40, 50), width=1)
    hero.save("screenshots/pulsar-time2-hero.png", "PNG")
    print("✓ Created screenshots/pulsar-time2-hero.png")

    # 2. 4-Platform Hardware Lineup (1280 x 580)
    line_w, line_h = 1280, 580
    lineup = Image.new('RGB', (line_w, line_h), (10, 10, 12))
    l_draw = ImageDraw.Draw(lineup)

    for x in range(0, line_w, 40):
        l_draw.line([(x, 0), (x, line_h)], fill=(18, 18, 22), width=1)
    for y in range(0, line_h, 40):
        l_draw.line([(0, y), (line_w, y)], fill=(18, 18, 22), width=1)

    try:
        font_ltitle = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 26)
        font_lsub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 13)
        font_llabel = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 14)
        font_lsublabel = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 12)
    except Exception:
        font_ltitle = font_lsub = font_llabel = font_lsublabel = ImageFont.load_default()

    l_draw.text((line_w//2, 30), "PEBBLE HARDWARE LINEUP", fill=(255, 255, 255), font=font_ltitle, anchor="mm")
    l_draw.text((line_w//2, 54), "AUTHENTIC PROCEDURAL GaAsP MATRIX RENDERING ON ALL PEBBLE PLATFORMS", fill=(220, 50, 50), font=font_lsub, anchor="mm")

    aplite_s = draw_pebble_aplite(scale=0.72, screen_image_path="screenshots/aplite-time.png")
    basalt_s = draw_pebble_basalt(scale=0.74, screen_image_path="screenshots/basalt-time.png")
    diorite_s = draw_pebble_diorite(scale=0.74, screen_image_path="screenshots/diorite-time.png")
    emery_s = draw_pebble_time2(scale=0.82, screen_image_path="screenshots/emery-time.png")

    devices = [
        ("APLITE", "Pebble Classic", aplite_s, 160),
        ("BASALT", "Pebble Time / Steel", basalt_s, 460),
        ("DIORITE", "Pebble 2 HR", diorite_s, 760),
        ("EMERY", "Pebble Time 2", emery_s, 1070),
    ]

    for plat, desc, dev_img, cx in devices:
        px = cx - dev_img.width // 2
        py = 65
        lineup.paste(dev_img, (px, py), dev_img)
        l_draw.text((cx, 510), plat, fill=(255, 255, 255) if plat=="EMERY" else (200, 200, 210), font=font_llabel, anchor="mm")
        l_draw.text((cx, 532), desc, fill=(200, 50, 50) if plat=="EMERY" else (140, 140, 150), font=font_lsublabel, anchor="mm")

    l_draw.rectangle([0, 0, line_w - 1, line_h - 1], outline=(40, 40, 50), width=1)
    lineup.save("screenshots/pulsar-all-platforms-mockup.png", "PNG")
    print("✓ Created screenshots/pulsar-all-platforms-mockup.png")

if __name__ == "__main__":
    generate_all()
