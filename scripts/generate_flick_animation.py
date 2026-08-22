#!/usr/bin/env python3
"""
generate_flick_animation.py

Generates a silky-smooth, studio-quality demonstration animation (MP4 + optimized GIF)
of the Pebble Time 2 demonstrating the wrist-flick gesture cycling through all 5 Pulsar 1970 modes:
Time (18:14) -> Seconds (:52) -> Date (08.21) -> Steps (08420) -> Battery (85%) -> Time.
Uses authentic Pebble Time 2 (Emery) hardware chassis.
"""

import os
import math
import shutil
from PIL import Image, ImageDraw, ImageEnhance, ImageFilter, ImageFont
from generate_hardware_mockups import draw_pebble_time2

def smooth_step(t):
    t = max(0.0, min(1.0, t))
    return t * t * (3 - 2 * t)

def ease_out_cubic(t):
    t = max(0.0, min(1.0, t))
    return 1.0 - math.pow(1.0 - t, 3)

def ease_in_cubic(t):
    t = max(0.0, min(1.0, t))
    return math.pow(t, 3)

def create_flick_animation():
    os.makedirs('screenshots/renders', exist_ok=True)
    temp_frames_dir = 'screenshots/renders/temp_frames'
    shutil.rmtree(temp_frames_dir, ignore_errors=True)
    os.makedirs(temp_frames_dir, exist_ok=True)

    # 1. Prepare base watch renders for each mode
    modes = [
        ("Time Mode", "18:14 GaAsP LEDs", "screenshots/emery-time.png"),
        ("Live Seconds", "Push-to-View Counter", "screenshots/emery-seconds.png"),
        ("Calendar Date", "Month & Day Display", "screenshots/emery-date.png"),
        ("Pebble Health", "Step Counter & Beads", "screenshots/emery-steps.png"),
        ("Battery State", "Power Fuel Gauge", "screenshots/emery-battery.png")
    ]

    mode_renders = []
    for title, sub, path in modes:
        watch_img = draw_pebble_time2(scale=1.2, screen_image_path=path)
        mode_renders.append((title, sub, watch_img))

    # Canvas dimensions for the animation (1080 x 1080)
    CANVAS_W, CANVAS_H = 1080, 1080
    bg_color = (12, 12, 15)

    try:
        font_mode = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 22)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial.ttf", 16)
        font_flick = ImageFont.truetype("/System/Library/Fonts/Supplemental/Courier New Bold.ttf", 16)
    except Exception:
        font_mode = font_sub = font_flick = ImageFont.load_default()

    frames = []
    frame_idx = 0

    print("Generating frames with authentic Pebble Time 2 hardware...")

    for i in range(len(modes)):
        cur_title, cur_sub, cur_img = mode_renders[i]
        next_i = (i + 1) % len(modes)
        next_title, next_sub, next_img = mode_renders[next_i]

        # Phase A: Stable Dwell (12 frames @ 20fps = 0.6s)
        for f in range(12):
            frame = Image.new('RGBA', (CANVAS_W, CANVAS_H), bg_color)
            f_draw = ImageDraw.Draw(frame)

            # Subtle radial background glow
            for r in range(400, 0, -10):
                alpha = int(18 * (1 - r / 400))
                f_draw.ellipse([CANVAS_W//2 - r, CANVAS_H//2 - r, CANVAS_W//2 + r, CANVAS_H//2 + r], fill=(alpha * 2, 0, 0))

            # Grid lines
            for gx in range(0, CANVAS_W, 40):
                f_draw.line([(gx, 0), (gx, CANVAS_H)], fill=(20, 20, 25), width=1)
            for gy in range(0, CANVAS_H, 40):
                f_draw.line([(0, gy), (CANVAS_W, gy)], fill=(20, 20, 25), width=1)

            # Paste watch centered
            wx = (CANVAS_W - cur_img.width) // 2
            wy = (CANVAS_H - cur_img.height) // 2 - 20
            frame.paste(cur_img, (wx, wy), cur_img)

            # Status overlay card at bottom
            card_w, card_h = 440, 75
            card_x = (CANVAS_W - card_w) // 2
            card_y = CANVAS_H - 140
            f_draw.rounded_rectangle([card_x, card_y, card_x + card_w, card_y + card_h], radius=12, fill=(20, 20, 26), outline=(45, 45, 55), width=1)
            f_draw.text((card_x + 24, card_y + 16), cur_title, fill=(255, 255, 255), font=font_mode)
            f_draw.text((card_x + 24, card_y + 44), cur_sub, fill=(220, 50, 50), font=font_sub)

            # Mode Dots (1..5)
            for d in range(5):
                dot_x = card_x + card_w - 110 + (d * 18)
                dot_y = card_y + 38
                dot_color = (230, 40, 40) if d == i else (50, 50, 60)
                f_draw.ellipse([dot_x - 5, dot_y - 5, dot_x + 5, dot_y + 5], fill=dot_color)

            frame_path = f"{temp_frames_dir}/frame_{frame_idx:04d}.png"
            frame.save(frame_path)
            frames.append(frame_path)
            frame_idx += 1

        # Phase B: Wrist Flick Transition (14 frames @ 20fps = 0.7s)
        # Flick motion: tilts right and away (tilt_angle), snaps back with next mode
        for f in range(14):
            t = f / 13.0
            
            # Physics: 0.0 -> 0.45 tilt away, 0.45 -> 1.0 snap back
            if t < 0.45:
                sub_t = t / 0.45
                tilt = -18.0 * ease_out_cubic(sub_t)
                scale_factor = 1.0 - (0.06 * ease_out_cubic(sub_t))
                display_img = cur_img
                title_disp, sub_disp = cur_title, cur_sub
            else:
                sub_t = (t - 0.45) / 0.55
                tilt = -18.0 * (1.0 - ease_in_cubic(sub_t))
                scale_factor = 0.94 + (0.06 * ease_in_cubic(sub_t))
                display_img = next_img
                title_disp, sub_disp = next_title, next_sub

            # Rotate watch slightly for 3D wrist flick effect
            rot_img = display_img.rotate(tilt, resample=Image.Resampling.BICUBIC, expand=True)

            frame = Image.new('RGBA', (CANVAS_W, CANVAS_H), bg_color)
            f_draw = ImageDraw.Draw(frame)

            # Background radial glow
            for r in range(400, 0, -10):
                alpha = int(18 * (1 - r / 400))
                f_draw.ellipse([CANVAS_W//2 - r, CANVAS_H//2 - r, CANVAS_W//2 + r, CANVAS_H//2 + r], fill=(alpha * 2, 0, 0))

            for gx in range(0, CANVAS_W, 40):
                f_draw.line([(gx, 0), (gx, CANVAS_H)], fill=(20, 20, 25), width=1)
            for gy in range(0, CANVAS_H, 40):
                f_draw.line([(0, gy), (CANVAS_W, gy)], fill=(20, 20, 25), width=1)

            # Paste rotated watch
            wx = (CANVAS_W - rot_img.width) // 2
            wy = (CANVAS_H - rot_img.height) // 2 - 20
            frame.paste(rot_img, (wx, wy), rot_img)

            # "⚡ WRIST FLICK" prompt indicator during flick peak
            if 0.25 <= t <= 0.75:
                flick_alpha = int(255 * math.sin((t - 0.25) / 0.5 * math.pi))
                pill_w, pill_h = 200, 36
                px = (CANVAS_W - pill_w) // 2
                py = 80
                f_draw.rounded_rectangle([px, py, px + pill_w, py + pill_h], radius=18, fill=(180, 25, 25), outline=(255, 80, 80), width=1)
                f_draw.text((px + 28, py + 9), "⚡ WRIST FLICK", fill=(255, 255, 255), font=font_flick)

            # Status overlay card at bottom
            card_w, card_h = 440, 75
            card_x = (CANVAS_W - card_w) // 2
            card_y = CANVAS_H - 140
            f_draw.rounded_rectangle([card_x, card_y, card_x + card_w, card_y + card_h], radius=12, fill=(20, 20, 26), outline=(45, 45, 55), width=1)
            f_draw.text((card_x + 24, card_y + 16), title_disp, fill=(255, 255, 255), font=font_mode)
            f_draw.text((card_x + 24, card_y + 44), sub_disp, fill=(220, 50, 50), font=font_sub)

            active_d = next_i if t >= 0.45 else i
            for d in range(5):
                dot_x = card_x + card_w - 110 + (d * 18)
                dot_y = card_y + 38
                dot_color = (230, 40, 40) if d == active_d else (50, 50, 60)
                f_draw.ellipse([dot_x - 5, dot_y - 5, dot_x + 5, dot_y + 5], fill=dot_color)

            frame_path = f"{temp_frames_dir}/frame_{frame_idx:04d}.png"
            frame.save(frame_path)
            frames.append(frame_path)
            frame_idx += 1

    print(f"Generated {len(frames)} total frames. Compiling MP4 and GIF...")

    # 3. Compile MP4 with ffmpeg
    mp4_out = "screenshots/renders/pebble-pulsar-flick-actions.mp4"
    gif_out = "screenshots/renders/pebble-pulsar-flick-actions-optimized.gif"

    # MP4
    os.system(f"ffmpeg -y -framerate 20 -i {temp_frames_dir}/frame_%04d.png -c:v libx264 -pix_fmt yuv420p -crf 18 {mp4_out}")
    print(f"✓ Saved MP4 video to {mp4_out}")

    # High quality GIF via color palette filter
    os.system(f"ffmpeg -y -framerate 20 -i {temp_frames_dir}/frame_%04d.png -vf 'scale=540:540:flags=lanczos,split[s0][s1];[s0]palettegen=max_colors=128:reserve_transparent=0[p];[s1][p]paletteuse=dither=bayer:bayer_scale=3' {gif_out}")
    print(f"✓ Saved optimized GIF to {gif_out}")

    shutil.rmtree(temp_frames_dir, ignore_errors=True)

if __name__ == "__main__":
    create_flick_animation()
