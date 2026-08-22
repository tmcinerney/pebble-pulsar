#!/usr/bin/env python3
"""
generate_flick_animation.py
Generates photorealistic animated demonstration clips (MP4 + GIF) of the 3D Pebble Time 2
demonstrating the wrist-flick gesture cycling through all 5 Pulsar 1970 display modes:
Time -> Seconds -> Date -> Steps -> Battery -> Time.
"""

import os
import math
import shutil
from PIL import Image, ImageDraw, ImageEnhance, ImageFont

def ease_out_elastic(t):
    """Elastic bounce physics curve for wrist flick return."""
    if t <= 0: return 0.0
    if t >= 1: return 1.0
    p = 0.3
    return math.pow(2, -10 * t) * math.sin((t - p / 4) * (2 * math.pi) / p) + 1.0

def ease_in_out(t):
    return 0.5 * (1 - math.cos(math.pi * t))

def create_flick_animation():
    os.makedirs('screenshots/renders', exist_ok=True)
    temp_frames_dir = 'screenshots/renders/temp_frames'
    shutil.rmtree(temp_frames_dir, ignore_errors=True)
    os.makedirs(temp_frames_dir, exist_ok=True)

    # 1. Load Base 3D Watch Render (Front view, 1024x1024)
    base_render_path = 'screenshots/renders/pebble-time2-3d-front.png'
    if not os.path.exists(base_render_path):
        print(f"Error: {base_render_path} not found.")
        return

    base_img = Image.open(base_render_path).convert('RGBA')
    W, H = base_img.size

    # Font setup for HUD badge
    try:
        font_badge = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 22)
    except Exception:
        font_badge = ImageFont.load_default()

    # Screen placement inside the 3D watch bezel
    screen_x, screen_y = 320, 276
    screen_w, screen_h = 384, 436
    corner_radius = 28

    # Create rounded corner screen mask
    mask = Image.new('L', (screen_w, screen_h), 0)
    mask_draw = ImageDraw.Draw(mask)
    mask_draw.rounded_rectangle([0, 0, screen_w, screen_h], radius=corner_radius, fill=255)

    # 2. Prepare the 5 Display Mode Textures from Actual Screenshots
    mode_files = [
        ("Time Mode", "screenshots/emery-time.png"),
        ("Live Seconds", "screenshots/emery-seconds.png"),
        ("Calendar Date", "screenshots/emery-date.png"),
        ("Daily Steps", "screenshots/emery-steps.png"),
        ("Battery Level", "screenshots/emery-battery.png"),
    ]

    mode_surfaces = []
    for name, path in mode_files:
        if os.path.exists(path):
            raw = Image.open(path).convert('RGBA')
            resized = raw.resize((screen_w, screen_h), Image.Resampling.LANCZOS)
            mode_surfaces.append((name, resized))

    if not mode_surfaces:
        print("No mode screenshots available.")
        return

    cycle_modes = mode_surfaces + [mode_surfaces[0]]

    # 3. Animation Timeline Parameters
    fps = 30
    dwell_frames = 42     # 1.4s per mode
    flick_frames = 14     # ~0.47s dynamic flick transition
    total_cycles = len(cycle_modes) - 1

    print(f"Rendering {total_cycles} flick transitions at {fps} FPS...")
    frame_idx = 0

    for cycle_i in range(total_cycles):
        curr_mode_name, curr_surf = cycle_modes[cycle_i]
        next_mode_name, next_surf = cycle_modes[cycle_i + 1]

        # --- A. Dwell / Idle Display State ---
        for f in range(dwell_frames):
            frame = base_img.copy()
            
            # Subtle glass reflection
            screen_comp = curr_surf.copy()
            glass_glare = Image.new('RGBA', (screen_w, screen_h), (0, 0, 0, 0))
            g_draw = ImageDraw.Draw(glass_glare)
            g_draw.polygon([(0, 0), (screen_w, 0), (screen_w // 2, screen_h), (0, screen_h // 2)], fill=(255, 255, 255, 12))
            screen_comp = Image.alpha_composite(screen_comp, glass_glare)

            frame.paste(screen_comp, (screen_x, screen_y), mask)

            # Mode Indicator Pill Badge at Top
            draw = ImageDraw.Draw(frame)
            draw.rounded_rectangle([W//2 - 140, 36, W//2 + 140, 84], radius=24, fill=(18, 18, 22, 240), outline=(180, 30, 30), width=2)
            draw.text((W//2, 60), curr_mode_name.upper(), fill=(255, 255, 255), font=font_badge, anchor="mm")

            frame.save(f"{temp_frames_dir}/frame_{frame_idx:05d}.png")
            frame_idx += 1

        # --- B. Wrist Flick Dynamic Motion Transition ---
        for f in range(flick_frames):
            t = f / float(flick_frames)
            
            if t < 0.35:
                sub_t = t / 0.35
                rot = -5.5 * ease_in_out(sub_t)
                dy = -22 * math.sin(sub_t * math.pi * 0.5)
                dx = 14 * math.sin(sub_t * math.pi * 0.5)
                glare_shift = sub_t * 60
                active_surf = curr_surf
                mode_label = curr_mode_name
                led_bloom = 1.0
            else:
                sub_t = (t - 0.35) / 0.65
                elastic = ease_out_elastic(sub_t)
                rot = -5.5 * (1.0 - elastic)
                dy = -22 * (1.0 - elastic)
                dx = 14 * (1.0 - elastic)
                glare_shift = 60 * (1.0 - sub_t)
                active_surf = next_surf
                mode_label = next_mode_name
                led_bloom = 1.0 + 0.35 * math.exp(-sub_t * 6.0)

            frame = base_img.copy()

            if led_bloom > 1.0:
                enhancer = ImageEnhance.Brightness(active_surf)
                boosted_surf = enhancer.enhance(led_bloom)
            else:
                boosted_surf = active_surf.copy()

            glass_glare = Image.new('RGBA', (screen_w, screen_h), (0, 0, 0, 0))
            g_draw = ImageDraw.Draw(glass_glare)
            g_offset = int(glare_shift)
            g_draw.polygon([(0 - g_offset, 0), (screen_w - g_offset, 0), (screen_w // 2 + g_offset, screen_h), (0 + g_offset, screen_h // 2)], fill=(255, 255, 255, int(15 + 25 * abs(rot)/5.5)))
            boosted_surf = Image.alpha_composite(boosted_surf, glass_glare)

            frame.paste(boosted_surf, (screen_x, screen_y), mask)

            # Apply 2D/3D wrist rotation and translation
            rotated_frame = frame.rotate(rot, resample=Image.Resampling.BICUBIC, center=(W//2, H//2), translate=(dx, dy))

            # Overlay HUD badge
            draw = ImageDraw.Draw(rotated_frame)
            draw.rounded_rectangle([W//2 - 140, 36, W//2 + 140, 84], radius=24, fill=(18, 18, 22, 240), outline=(240, 45, 45) if t >= 0.35 else (180, 30, 30), width=2)
            badge_text = f"FLICK ➔ {mode_label.upper()}" if t >= 0.35 else "WRIST FLICK..."
            draw.text((W//2, 60), badge_text, fill=(255, 255, 255) if t < 0.35 else (255, 110, 110), font=font_badge, anchor="mm")

            rotated_frame.save(f"{temp_frames_dir}/frame_{frame_idx:05d}.png")
            frame_idx += 1

    print(f"Generated {frame_idx} frames. Compiling MP4 and GIF video clips with ffmpeg...")

    # 4. Compile High-Def MP4 (1080p, H.264, 30fps)
    mp4_out = 'screenshots/renders/pebble-pulsar-flick-actions.mp4'
    cmd_mp4 = f"ffmpeg -y -r {fps} -i {temp_frames_dir}/frame_%05d.png -c:v libx264 -pix_fmt yuv420p -crf 18 -preset slow {mp4_out}"
    os.system(cmd_mp4)
    print(f"✓ Created MP4 clip: {mp4_out}")

    # 5. Compile Optimized High-Quality Animated GIF (500x500 for GitHub / Web)
    gif_out = 'screenshots/renders/pebble-pulsar-flick-actions.gif'
    palette_path = f"{temp_frames_dir}/palette.png"
    cmd_palette = f"ffmpeg -y -i {temp_frames_dir}/frame_%05d.png -vf 'fps=20,scale=500:500:flags=lanczos,palettegen=stats_mode=diff' -update 1 {palette_path}"
    os.system(cmd_palette)
    cmd_gif = f"ffmpeg -y -r 20 -i {temp_frames_dir}/frame_%05d.png -i {palette_path} -lavfi 'fps=20,scale=500:500:flags=lanczos [x]; [x][1:v] paletteuse=dither=bayer:bayer_scale=3' {gif_out}"
    os.system(cmd_gif)
    print(f"✓ Created Animated GIF: {gif_out}")

    # Clean up temp frames
    shutil.rmtree(temp_frames_dir, ignore_errors=True)
    print("✓ Rendering and compression pipeline complete!")

if __name__ == '__main__':
    create_flick_animation()
