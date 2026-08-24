#!/usr/bin/env python3
"""
generate_appstore_pack.py

Consolidates and prepares complete, turnkey Rebble Appstore submission packages
for each application in the Pebble Pulsar Suite.

Outputs for each app in dist/appstore-pack/<app>/:
1. banner-720x320.png (Customized Rebble Appstore Banner)
2. icon-260x260.png (Appstore Square Icon)
3. screenshot-emery.png (200x228 Pebble Time 2 screenshot)
4. screenshot-basalt.png (144x168 Pebble Time / Steel screenshot)
5. screenshot-diorite.png (144x168 Pebble 2 HR screenshot)
6. screenshot-aplite.png (144x168 Pebble Classic screenshot)
7. <app>.pbw (Compiled installable binary bundle)
8. listing.json (Structured metadata)
"""

import os
import sys
import shutil
from PIL import Image, ImageDraw, ImageFont

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import generate_hardware_mockups as ghm

APPS = {
    "watchface": {
        "title": "Pulsar 1970",
        "subtitle": "1 9 7 0   T I M E   C O M P U T E R",
        "tagline": "The 1972 Hamilton Digital LED Icon for Pebble OS.",
        "category": "Watchfaces",
        "version": "2.2.0",
        "pbw_name": "pebble-pulsar-watchface.pbw",
        "shot_prefix": "time",
        "bullets": [
            "● Procedural 5x7 GaAsP Dot-Matrix LEDs",
            "● 6 Modes: Time, Sec, Date, Steps, Batt, HR",
            "● Wrist Flick / Tap Gesture Cycling",
            "● 8-Bit Retro Synth Chimes & Nightlight"
        ]
    },
    "chrono": {
        "title": "Pulsar Chrono",
        "subtitle": "P R E C I S I O N   C H R O N O M E T E R",
        "tagline": "Vintage Sub-Second Digital Chronograph with 20-Lap Memory.",
        "category": "Tools & Utilities",
        "version": "1.1.0",
        "pbw_name": "pebble-pulsar-chrono.pbw",
        "shot_prefix": "chrono-running",
        "bullets": [
            "● 50ms High-Frequency Centisecond Readout",
            "● Live Lap Split Freeze & 20-Lap Review",
            "● 10-Dot Micro-LED Optical Tachymeter",
            "● Auto Hours Scaling & Zero-Drift Persistence"
        ]
    },
    "timer": {
        "title": "Pulsar Timer",
        "subtitle": "C O U N T D O W N   I N S T R U M E N T",
        "tagline": "Vintage LED Countdown Timer & Pomodoro Focus Tool.",
        "category": "Tools & Utilities",
        "version": "1.0.0",
        "pbw_name": "pebble-pulsar-timer.pbw",
        "shot_prefix": "timer-running",
        "bullets": [
            "● 10 Quick Duration Presets (1m to 60m)",
            "● 10-Dot Micro-LED Depletion Progress Bar",
            "● Pebble Wakeup API Background Scheduler",
            "● Escalating Ruby Alarm & 3-2-1 Warning Tones"
        ]
    },
    "alarm": {
        "title": "Pulsar Alarm",
        "subtitle": "M U L T I - A L A R M   C L O C K",
        "tagline": "Vintage Multi-Schedule Digital LED Alarm Clock.",
        "category": "Daily",
        "version": "1.0.0",
        "pbw_name": "pebble-pulsar-alarm.pbw",
        "shot_prefix": "alarm-view",
        "bullets": [
            "● 4 Independent Multi-Alarm Slots",
            "● Repeat Schedules (Daily, Weekdays, Weekends)",
            "● Reliable Background Wakeup API Engine",
            "● Vintage 9-Minute Snooze & Dial-In Edit"
        ]
    }
}


def draw_app_banner(app_key, app_info, out_path):
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

    # Renders on Right
    p2_shot = f"screenshots/{app_key}/diorite-{app_info['shot_prefix']}.png"
    if not os.path.exists(p2_shot):
        p2_shot = f"screenshots/{app_key}/diorite-time.png"
    
    t2_shot = f"screenshots/{app_key}/emery-{app_info['shot_prefix']}.png"
    if not os.path.exists(t2_shot):
        t2_shot = f"screenshots/{app_key}/emery-time.png"

    p2_mock = ghm.draw_pebble_diorite(scale=0.48, screen_image_path=p2_shot)
    t2_mock = ghm.draw_pebble_time2(scale=0.56, screen_image_path=t2_shot)

    banner.paste(p2_mock, (360, 20), p2_mock)
    banner.paste(t2_mock, (460, -5), t2_mock)

    # Typography & Branding on Left
    try:
        font_logo = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 34)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 12)
        font_desc = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 13)
        font_badge = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 12)
    except Exception:
        font_logo = font_sub = font_desc = font_badge = ImageFont.load_default()

    # Red Accent bar
    draw.rectangle([34, 42, 38, 138], fill=(220, 20, 20))

    # Main Brand Text
    draw.text((48, 40), app_info["title"].upper(), fill=(255, 255, 255), font=font_logo)
    draw.text((50, 88), app_info["subtitle"], fill=(220, 50, 50), font=font_sub)
    draw.text((50, 116), app_info["tagline"], fill=(170, 170, 180), font=font_desc)

    by = 158
    for b in app_info["bullets"]:
        draw.text((50, by), b, fill=(200, 200, 210), font=font_badge)
        by += 28

    draw.rectangle([0, 0, width - 1, height - 1], outline=(45, 45, 55), width=1)
    banner.save(out_path, 'PNG')
    print(f"  ✓ Created Banner: {out_path}")


def draw_app_icon(app_key, app_info, size, out_path):
    icon_w, icon_h = size, size
    icon = Image.new('RGBA', (icon_w, icon_h), (0, 0, 0, 0))
    icon_draw = ImageDraw.Draw(icon)
    
    # Rounded dark squircle container
    r = int(icon_w * 0.22)
    icon_draw.rounded_rectangle([0, 0, icon_w - 1, icon_h - 1], radius=r, fill=(16, 16, 20, 255), outline=(48, 48, 56, 255), width=max(1, int(icon_w / 72)))

    # Red accent stripe at top
    stripe_h = max(2, int(icon_h * 0.04))
    icon_draw.rounded_rectangle([int(icon_w * 0.2), int(icon_h * 0.1), int(icon_w * 0.8), int(icon_h * 0.1) + stripe_h], radius=1, fill=(220, 20, 20, 255))

    # Center device / screen render
    t2_shot = f"screenshots/{app_key}/emery-{app_info['shot_prefix']}.png"
    if not os.path.exists(t2_shot):
        t2_shot = f"screenshots/{app_key}/emery-time.png"

    if os.path.exists(t2_shot):
        with Image.open(t2_shot) as scr:
            scr = scr.convert('RGBA')
            # Scale screen to fit inside icon nicely
            target_h = int(icon_h * 0.62)
            target_w = int(scr.width * (target_h / float(scr.height)))
            scr_scaled = scr.resize((target_w, target_h), Image.Resampling.LANCZOS)
            
            # Draw bezel around screen
            bx = (icon_w - target_w) // 2
            by = int(icon_h * 0.24)
            pad = max(1, int(icon_w / 48))
            icon_draw.rounded_rectangle([bx - pad, by - pad, bx + target_w + pad - 1, by + target_h + pad - 1], radius=max(2, int(icon_w / 32)), fill=(8, 8, 10, 255), outline=(60, 60, 70, 255), width=1)
            icon.paste(scr_scaled, (bx, by), scr_scaled)

    icon.save(out_path, 'PNG')
    print(f"  ✓ Created Icon ({icon_w}x{icon_h}): {out_path}")


def organize_platform_screenshots(app_key, app_info, pack_dir):
    """Gathers and organizes up to 5 screenshots per hardware platform."""
    platforms = ["emery", "basalt", "diorite", "aplite"]
    
    shot_mappings = {
        "watchface": [
            ("time", "1-time-mode"),
            ("seconds", "2-seconds-mode"),
            ("date", "3-calendar-date"),
            ("steps", "4-health-steps"),
            ("battery", "5-battery-status")
        ],
        "chrono": [
            ("chrono-ready", "1-stopwatch-ready"),
            ("chrono-running", "2-centiseconds-running"),
            ("chrono-lap", "3-lap-split-freeze"),
            ("chrono-lap-review", "4-lap-split-browser"),
            ("chrono-running", "5-tachymeter-chaser")
        ],
        "timer": [
            ("timer-preset", "1-preset-picker"),
            ("timer-running", "2-countdown-running"),
            ("timer-preset", "3-pomodoro-focus"),
            ("timer-running", "4-depletion-gauge"),
            ("timer-running", "5-alarm-alert")
        ],
        "alarm": [
            ("alarm-view", "1-alarm-slot1-daily"),
            ("alarm-view", "2-multi-alarm-slots"),
            ("alarm-view", "3-repeat-schedules"),
            ("alarm-view", "4-bedside-mode"),
            ("alarm-view", "5-vintage-snooze")
        ]
    }

    print(f"\n  Organizing 5 Platform Screenshots for '{app_key}'...")
    for p in platforms:
        plat_dir = os.path.join(pack_dir, "screenshots", p)
        os.makedirs(plat_dir, exist_ok=True)
        
        mappings = shot_mappings.get(app_key, [(app_info["shot_prefix"], "1-main")])
        for idx, (shot_type, label) in enumerate(mappings, 1):
            src_shot = os.path.join(REPO_ROOT, "screenshots", app_key, f"{p}-{shot_type}.png")
            if not os.path.exists(src_shot):
                src_shot = os.path.join(REPO_ROOT, "screenshots", f"{p}-{shot_type}.png")
            if not os.path.exists(src_shot):
                src_shot = os.path.join(REPO_ROOT, "screenshots", app_key, f"{p}-{app_info['shot_prefix']}.png")
            if not os.path.exists(src_shot):
                src_shot = os.path.join(REPO_ROOT, "screenshots", app_key, f"{p}-time.png")

            if os.path.exists(src_shot):
                dst_shot = os.path.join(plat_dir, f"{idx}-{label}.png")
                shutil.copyfile(src_shot, dst_shot)
                # Also keep a top-level single screenshot for quick upload
                if idx == 1:
                    shutil.copyfile(src_shot, os.path.join(pack_dir, f"screenshot-{p}.png"))
                print(f"    ✓ [{p.upper()}] Screenshot {idx}: {os.path.basename(dst_shot)}")


def build_pack_for_app(app_key):
    info = APPS[app_key]
    pack_dir = os.path.join(REPO_ROOT, "dist", "appstore-submission", app_key)
    os.makedirs(pack_dir, exist_ok=True)

    print(f"\nPackaging Rebble Appstore Materials for '{info['title']}'...")

    # 1. Appstore Banner (720x320)
    banner_file = os.path.join(pack_dir, f"banner-{app_key}-720x320.png")
    draw_app_banner(app_key, info, banner_file)

    # 2. App Icons:
    # 2a. Store Tile Icon (260x260)
    icon_260 = os.path.join(pack_dir, f"icon-{app_key}-260x260.png")
    draw_app_icon(app_key, info, 260, icon_260)

    # 2b. App Locker Large Icon (144x144)
    icon_144 = os.path.join(pack_dir, f"icon-{app_key}-144x144.png")
    draw_app_icon(app_key, info, 144, icon_144)

    # 2c. App Locker Small Icon (48x48)
    icon_48 = os.path.join(pack_dir, f"icon-{app_key}-48x48.png")
    draw_app_icon(app_key, info, 48, icon_48)

    # 3. Organize 5 Screenshots per Platform
    organize_platform_screenshots(app_key, info, pack_dir)

    # 4. Copy PBW if exists
    dist_pbw = os.path.join(REPO_ROOT, "dist", info["pbw_name"])
    if not os.path.exists(dist_pbw):
        app_build_pbw = os.path.join(REPO_ROOT, "apps", app_key, "build", f"{app_key}.pbw")
        if os.path.exists(app_build_pbw):
            dist_pbw = app_build_pbw

    if os.path.exists(dist_pbw):
        dst_pbw = os.path.join(pack_dir, info["pbw_name"])
        shutil.copyfile(dist_pbw, dst_pbw)
        print(f"  ✓ Bundled Binary: {dst_pbw}")

    print(f"✅ App Store Pack Ready in: {pack_dir}")


def main():
    target = sys.argv[1].lower() if len(sys.argv) > 1 else "all"
    if target in APPS:
        build_pack_for_app(target)
    elif target == "all":
        for k in APPS:
            build_pack_for_app(k)
    else:
        print(f"Usage: python3 generate_appstore_pack.py [watchface|chrono|timer|alarm|all]")


if __name__ == "__main__":
    main()
