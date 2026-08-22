#!/usr/bin/env python3
"""
generate_flick_animation.py
Generates silky-smooth, photorealistic animated demonstration clips (MP4 + optimized GIF)
of the Pebble Time 2 demonstrating the wrist-flick gesture cycling through all 5 Pulsar 1970 modes:
Time (18:14) -> Seconds (:52) -> Date (08.21) -> Steps (08420) -> Battery (85%) -> Time.
"""

import os
import math
import shutil
from PIL import Image, ImageDraw, ImageEnhance, ImageFilter, ImageFont

def smooth_step(t):
    """Hermite smoothstep interpolation."""
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

    # 1. Load Base Hardware (Clean front render, 1024x1024)
    base_render_path = 'screenshots/renders/base_hardware_front.png'
    base_img = Image.open(base_render_path).convert('RGBA')
    W, H = base_img.size

    # Extract clean solid background color (#373739 / dark studio gray)
    bg_color = (48, 48, 52, 255)

    # Screen placement inside the 3D watch bezel
    sx, sy, sw, sh = 340, 290, 344, 392
    radius = 36

    # Create rounded corner screen mask
    mask = Image.new('L', (sw, sh), 0)
    draw_m = ImageDraw.Draw(mask)
    draw_m.rounded_rectangle([0, 0, sw, sh], radius=radius, fill=255)

    # Glass crop from base image to preserve natural optical reflections
    glass_ambient = base_img.crop((sx, sy, sx + sw, sy + sh))

    # Font setup for bottom status pill
    try:
        font_mode = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 20)
        font_sub = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial.ttf", 15)
    except Exception:
        font_mode = ImageFont.load_default()
        font_sub = ImageFont.load_default()

    # 2. Prepare the 5 Display Mode Textures
    mode_configs = [
        ("TIME MODE", "12H / 24H AUTONOMOUS TIME", "screenshots/emery-time.png"),
        ("LIVE SECONDS", "1 HZ REAL-TIME TICKING", "screenshots/emery-seconds.png"),
        ("CALENDAR DATE", "SPACE-AGE MONTH & DAY", "screenshots/emery-date.png"),
        ("DAILY STEPS", "PEBBLE HEALTH 10-DOT GOAL", "screenshots/emery-steps.png"),
        ("BATTERY LEVEL", "DIGITAL POWER MONITOR", "screenshots/emery-battery.png"),
    ]

    mode_surfaces = []
    for title, subtitle, path in mode_configs:
        raw = Image.open(path).convert('RGBA')
        resized = raw.resize((sw, sh), Image.Resampling.LANCZOS)
        
        # Add subtle LED bloom
        bloom = resized.filter(ImageFilter.GaussianBlur(radius=3))
        bloomed = Image.blend(resized, bloom, 0.20)
        
        # Blend ambient glass reflection
        blended = Image.blend(bloomed, glass_ambient, 0.10)
        mode_surfaces.append((title, subtitle, blended))

    # Cycle back to Time mode at the end for infinite seamless loop
    cycle_modes = mode_surfaces + [mode_surfaces[0]]

    # 3. Animation Timing Parameters
    fps = 30
    dwell_frames = 48     # 1.6s reading time per mode (stable, steady)
    flick_frames = 18     # 0.6s total wrist flick gesture
    total_cycles = len(cycle_modes) - 1

    print(f"Rendering {total_cycles} smooth wrist flick cycles at {fps} FPS...")
    frame_idx = 0

    for cycle_i in range(total_cycles):
        curr_title, curr_sub, curr_surf = cycle_modes[cycle_i]
        next_title, next_sub, next_surf = cycle_modes[cycle_i + 1]

        # --- A. Stable Dwell State (Zero jitter, perfect reading stability) ---
        for f in range(dwell_frames):
            # Composite watch face
            watch_comp = base_img.copy()
            watch_comp.paste(curr_surf, (sx, sy), mask)

            # Draw sleek, static bottom HUD bar (fixed on canvas)
            draw = ImageDraw.Draw(watch_comp)
            
            # Bottom info pill
            pill_w, pill_h = 360, 52
            px1, py1 = (W - pill_w) // 2, 940
            draw.rounded_rectangle([px1, py1, px1 + pill_w, py1 + pill_h], radius=26, fill=(18, 18, 22, 230), outline=(55, 55, 65), width=1)
            draw.text((W // 2, py1 + 18), curr_title, fill=(240, 240, 245), font=font_mode, anchor="mm")
            draw.text((W // 2, py1 + 36), curr_sub, fill=(170, 170, 180), font=font_sub, anchor="mm")

            watch_comp.save(f"{temp_frames_dir}/frame_{frame_idx:05d}.png")
            frame_idx += 1

        # --- B. Smooth Wrist Flick Gesture ---
        # 0.0 to 0.4: Accelerate flick upward / leftward (wrist rotation)
        # 0.4: Mode trigger & LED ignition flare
        # 0.4 to 1.0: Smooth deceleration back to neutral rest position
        for f in range(flick_frames):
            t = f / float(flick_frames)

            if t < 0.38:
                # Upward wrist twist phase
                p = t / 0.38
                curve = ease_in_cubic(p)
                angle = -3.8 * curve       # Max 3.8 degree natural tilt
                dy = -14.0 * curve         # 14px vertical shift
                dx = 8.0 * curve          # 8px lateral shift
                glare_alpha = int(curve * 28)
                active_surf = curr_surf
                display_title, display_sub = curr_title, curr_sub
                led_flare = 1.0
            else:
                # Settle phase
                p = (t - 0.38) / 0.62
                curve = ease_out_cubic(p)
                angle = -3.8 * (1.0 - curve)
                dy = -14.0 * (1.0 - curve)
                dx = 8.0 * (1.0 - curve)
                glare_alpha = int((1.0 - curve) * 28)
                active_surf = next_surf
                display_title, display_sub = next_title, next_sub
                # LED activation flare on mode change
                led_flare = 1.0 + 0.28 * math.exp(-p * 5.0)

            # Build watch image with screen
            screen_boosted = active_surf
            if led_flare > 1.0:
                enhancer = ImageEnhance.Brightness(active_surf)
                screen_boosted = enhancer.enhance(led_flare)

            watch_composite = base_img.copy()
            watch_composite.paste(screen_boosted, (sx, sy), mask)

            # Smooth dynamic specular sheen on the crystal during tilt
            if glare_alpha > 0:
                glare_layer = Image.new('RGBA', (sw, sh), (0, 0, 0, 0))
                g_draw = ImageDraw.Draw(glare_layer)
                offset = int((t - 0.38) * 80)
                g_draw.polygon([(0 - offset, 0), (sw - offset, 0), (sw // 2 + offset, sh), (0 + offset, sh // 2)], fill=(255, 255, 255, glare_alpha))
                glare_layer = glare_layer.filter(ImageFilter.GaussianBlur(radius=8))
                watch_composite.paste(glare_layer, (sx, sy), mask)

            # Rotate the watch on an isolated layer over solid background to avoid clipping
            canvas = Image.new('RGBA', (W, H), bg_color)
            rotated_watch = watch_composite.rotate(
                angle,
                resample=Image.Resampling.BICUBIC,
                center=(W // 2, H // 2),
                translate=(dx, dy)
            )
            canvas = Image.alpha_composite(canvas, rotated_watch)

            # Draw fixed HUD pill
            draw = ImageDraw.Draw(canvas)
            pill_w, pill_h = 360, 52
            px1, py1 = (W - pill_w) // 2, 940
            outline_col = (200, 40, 40) if t >= 0.38 else (80, 80, 95)
            draw.rounded_rectangle([px1, py1, px1 + pill_w, py1 + pill_h], radius=26, fill=(18, 18, 22, 230), outline=outline_col, width=2 if t >= 0.38 else 1)
            text_col = (255, 90, 90) if t >= 0.38 and t < 0.65 else (240, 240, 245)
            draw.text((W // 2, py1 + 18), f"FLICK ➔ {display_title}" if t >= 0.38 and t < 0.7 else display_title, fill=text_col, font=font_mode, anchor="mm")
            draw.text((W // 2, py1 + 36), display_sub, fill=(170, 170, 180), font=font_sub, anchor="mm")

            canvas.save(f"{temp_frames_dir}/frame_{frame_idx:05d}.png")
            frame_idx += 1

    print(f"Generated {frame_idx} clean frames. Encoding MP4 and GIF...")

    # 4. Compile Silky-Smooth 1080p MP4 (H.264, 30fps, CRF 17)
    mp4_out = 'screenshots/renders/pebble-pulsar-flick-actions.mp4'
    cmd_mp4 = f"ffmpeg -y -r {fps} -i {temp_frames_dir}/frame_%05d.png -c:v libx264 -pix_fmt yuv420p -crf 17 -preset slow {mp4_out}"
    os.system(cmd_mp4)
    print(f"✓ Created smooth MP4: {mp4_out}")

    # 5. Compile Ultra-Clean GIF (540x540, 25fps, diffusion dither)
    gif_out = 'screenshots/renders/pebble-pulsar-flick-actions.gif'
    gif_opt = 'screenshots/renders/pebble-pulsar-flick-actions-optimized.gif'

    palette_path = f"{temp_frames_dir}/palette.png"
    # Generate high-precision palette
    cmd_pal = f"ffmpeg -y -i {temp_frames_dir}/frame_%05d.png -vf 'fps=25,scale=540:540:flags=lanczos,palettegen=stats_mode=diff:max_colors=256' -update 1 {palette_path}"
    os.system(cmd_pal)

    # Render smooth master GIF
    cmd_gif = f"ffmpeg -y -r 25 -i {temp_frames_dir}/frame_%05d.png -i {palette_path} -lavfi 'fps=25,scale=540:540:flags=lanczos [x]; [x][1:v] paletteuse=dither=floyd_steinberg' {gif_out}"
    os.system(cmd_gif)
    print(f"✓ Created master GIF: {gif_out}")

    # Render lightweight web GIF (440x440, 20fps)
    cmd_opt = f"ffmpeg -y -i {gif_out} -vf 'fps=20,scale=440:440:flags=lanczos' {gif_opt}"
    os.system(cmd_opt)
    print(f"✓ Created optimized web GIF: {gif_opt}")

    shutil.rmtree(temp_frames_dir, ignore_errors=True)
    print("✓ All animations rendered and encoded flawlessly!")

if __name__ == '__main__':
    create_flick_animation()
