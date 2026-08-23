#!/usr/bin/env python3
"""
High-Fidelity, Frame-Perfect Animated GIF Generator for Pebble Pulsar 1970.
Renders smooth, high-framerate, mathematically exact animations matching pulsar.c firmware:
1. emery-charging-cylon.gif (Smooth 30 FPS Cylon 3-dot ping-pong sweep)
2. emery-charging-marquee.gif (Smooth 20 FPS 1970s Theater Marquee alternating LEDs)
3. emery-steps-overdrive.gif (Smooth 20 FPS Step Overdrive celebration with pulsing Lap 2 beads)
4. pebble-pulsar-flick-actions-optimized.gif (Showcase hero GIF with device framing, ticking seconds, beating heart, and mode transitions)
"""

import os
import math
from PIL import Image, ImageDraw

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCREENSHOTS_DIR = os.path.join(REPO_ROOT, "screenshots")
RENDERS_DIR = os.path.join(SCREENSHOTS_DIR, "renders")
MOCKUPS_DIR = os.path.join(SCREENSHOTS_DIR, "mockups")

os.makedirs(SCREENSHOTS_DIR, exist_ok=True)
os.makedirs(RENDERS_DIR, exist_ok=True)
os.makedirs(MOCKUPS_DIR, exist_ok=True)

# 5x7 GaAsP LED Matrix Font definition from pulsar.c
FONT_5X7 = [
    [0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F], # 0
    [0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E], # 1
    [0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F], # 2
    [0x1F, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x1F], # 3
    [0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01], # 4
    [0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F], # 5
    [0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F], # 6
    [0x1F, 0x01, 0x01, 0x02, 0x04, 0x04, 0x04], # 7
    [0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F], # 8
    [0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F], # 9
    [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00], # 10 = Blank
    [0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E], # 11 = 'B'
    [0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11], # 12 = 'A'
    [0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04], # 13 = 'T'
    [0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03], # 14 = '%'
    [0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11], # 15 = 'H'
    [0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11], # 16 = 'R'
    [0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00], # 17 = '-'
    [0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00]  # 18 = '♥' (Heart)
]

# Color constants
COLOR_LIT_RED = (255, 32, 32, 255)       # Bright GaAsP Neon Ruby Red
COLOR_GHOST_RED = (85, 0, 0, 255)         # Deep maroon unlit ghost matrix
COLOR_TEXT_WHITE = (255, 255, 255, 255)   # Header / Footer text
COLOR_BG_BLACK = (0, 0, 0, 255)           # Pitch black background

def extract_header_footer(screenshot_path):
    """Extracts top header and bottom footer rasters from actual Pebble screenshots for 100% authentic typography."""
    img = Image.open(screenshot_path).convert("RGBA")
    # Top header (y: 0 to 36)
    header = img.crop((0, 0, 200, 36))
    # Bottom footer (y: 180 to 228)
    footer = img.crop((0, 180, 200, 228))
    return header, footer

# Load authentic text overlays from existing verified screenshots
HEADER_PULSAR, FOOTER_TIME = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-time.png"))
_, FOOTER_SECONDS = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-seconds.png"))
_, FOOTER_DATE = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-date.png"))
_, FOOTER_STEPS = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-steps.png"))
_, FOOTER_BATTERY = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-battery.png"))
_, FOOTER_HEART = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-heartrate.png"))
_, FOOTER_CYLON = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-charging-cylon.png"))
_, FOOTER_MARQUEE = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-charging-marquee.png"))
_, FOOTER_OVERDRIVE = extract_header_footer(os.path.join(SCREENSHOTS_DIR, "emery-steps-overdrive.png"))

def draw_matrix_digit(draw, x_offset, y_offset, digit_index, spacing_x=8, spacing_y=9, dot_radius=2, slant_scale=7, lit_color=COLOR_LIT_RED, ghost_color=COLOR_GHOST_RED, is_active=True):
    if digit_index < 0 or digit_index > 18:
        digit_index = 10
        
    for r in range(7):
        row_bits = FONT_5X7[digit_index][r]
        slant_x = ((6 - r) * slant_scale) // 6 if slant_scale > 0 else 0
        
        for c in range(5):
            is_lit = is_active and ((row_bits >> (4 - c)) & 0x01)
            dot_x = x_offset + (c * spacing_x) + slant_x
            dot_y = y_offset + (r * spacing_y)
            
            if is_lit:
                draw.ellipse([dot_x - dot_radius, dot_y - dot_radius, dot_x + dot_radius, dot_y + dot_radius], fill=lit_color)
            else:
                if ghost_color is not None:
                    draw.ellipse([dot_x - 1, dot_y - 1, dot_x + 1, dot_y + 1], fill=ghost_color)

def render_emery_screen(d1=10, d2=10, d3=10, d4=10, colon=False, footer_img=FOOTER_TIME, 
                         beads_lit=None, bead_pulse_idx=-1, bead_pulse_intensity=1.0,
                         charging_dot=False, bt_dot=False, is_5_digit=False, d5=10,
                         heart_scale=1.0, heart_intensity=1.0):
    """Renders a single frame of Pebble Time 2 (200x228) with authentic GaAsP optics."""
    img = Image.new("RGBA", (200, 228), COLOR_BG_BLACK)
    draw = ImageDraw.Draw(img)
    
    # 1. Header
    img.paste(HEADER_PULSAR, (0, 0), HEADER_PULSAR)
    
    # 2. Status Annunciator Dots
    if bt_dot:
        draw.ellipse([18 - 2, 20 - 2, 18 + 2, 20 + 2], fill=COLOR_LIT_RED)
    if charging_dot:
        draw.ellipse([182 - 2, 20 - 2, 182 + 2, 20 + 2], fill=COLOR_LIT_RED)
        
    # 3. Main Digits
    start_y = 70
    slant_scale = 7
    digit_span_x = 4 * 8 # 32
    digit_gap = 14
    colon_gap = 22
    total_4_width = (4 * digit_span_x) + (2 * digit_gap) + colon_gap + slant_scale
    start_4_x = (200 - total_4_width) // 2
    
    if is_5_digit:
        # 5-digit layout (Steps / Heart Rate)
        step_spacing_x = 6
        step_spacing_y = 7
        step_dot_radius = 2
        step_gap = 11
        step_digit_span = 4 * step_spacing_x # 24
        total_5_width = (5 * step_digit_span) + (4 * step_gap) + slant_scale
        start_5_x = (200 - total_5_width) // 2
        step_start_y = 72
        
        digits = [d1, d2, d3, d4, d5]
        for i, dig in enumerate(digits):
            dx = start_5_x + (i * (step_digit_span + step_gap))
            cur_lit = COLOR_LIT_RED
            if dig == 18: # Heart
                r = min(255, int(255 * heart_intensity))
                g = min(255, int(32 * heart_intensity))
                b = min(255, int(32 * heart_intensity))
                cur_lit = (r, g, b, 255)
            draw_matrix_digit(draw, dx, step_start_y, dig, spacing_x=step_spacing_x, spacing_y=step_spacing_y,
                              dot_radius=step_dot_radius, slant_scale=slant_scale, lit_color=cur_lit)
    else:
        # 4-digit layout (Time, Seconds, Date, Battery)
        d1_x = start_4_x
        d2_x = d1_x + digit_span_x + digit_gap
        colon_x = d2_x + digit_span_x + (colon_gap // 2)
        d3_x = d2_x + digit_span_x + colon_gap
        d4_x = d3_x + digit_span_x + digit_gap
        
        draw_matrix_digit(draw, d1_x, start_y, d1, spacing_x=8, spacing_y=9, dot_radius=2, slant_scale=slant_scale)
        draw_matrix_digit(draw, d2_x, start_y, d2, spacing_x=8, spacing_y=9, dot_radius=2, slant_scale=slant_scale)
        draw_matrix_digit(draw, d3_x, start_y, d3, spacing_x=8, spacing_y=9, dot_radius=2, slant_scale=slant_scale)
        draw_matrix_digit(draw, d4_x, start_y, d4, spacing_x=8, spacing_y=9, dot_radius=2, slant_scale=slant_scale)
        
        # Blinking / Active Colon
        for r_c in [2, 4]:
            colon_slant = ((6 - r_c) * slant_scale) // 6
            cx = colon_x + colon_slant
            cy = start_y + (9 * r_c)
            if colon:
                draw.ellipse([cx - 2, cy - 2, cx + 2, cy + 2], fill=COLOR_LIT_RED)
            else:
                draw.ellipse([cx - 1, cy - 1, cx + 1, cy + 1], fill=COLOR_GHOST_RED)
                
    # 4. Step / Status Beads
    if beads_lit is not None:
        bead_y = 148
        bead_spacing = 10
        total_bead_w = 9 * bead_spacing
        start_bx = (200 - total_bead_w) // 2
        for b_idx in range(10):
            bx = start_bx + (b_idx * bead_spacing)
            is_lit = beads_lit[b_idx] if b_idx < len(beads_lit) else False
            
            if is_lit:
                if b_idx <= bead_pulse_idx:
                    r = min(255, int(255 * bead_pulse_intensity))
                    g = min(255, int(32 * bead_pulse_intensity))
                    b = min(255, int(32 * bead_pulse_intensity))
                    fill_c = (r, g, b, 255)
                    rad = 2
                else:
                    fill_c = COLOR_LIT_RED
                    rad = 2
                draw.ellipse([bx - rad, bead_y - rad, bx + rad, bead_y + rad], fill=fill_c)
            else:
                draw.ellipse([bx - 1, bead_y - 1, bx + 1, bead_y + 1], fill=COLOR_GHOST_RED)
                
    # 5. Footer Text
    if footer_img:
        img.paste(footer_img, (0, 180), footer_img)
        
    return img

def render_cylon_animation():
    """Generates ultra-smooth 30 FPS Cylon Charging Animation."""
    print("Generating High-Fidelity 30 FPS Cylon Charging GIF...", flush=True)
    fps = 30
    duration_sec = 2.0
    total_frames = int(fps * duration_sec) # 60 frames
    frames = []
    
    for f in range(total_frames):
        t = f / total_frames # 0.0 to 1.0
        # 18-step ping pong cycle
        step_float = t * 18.0
        step = int(step_float)
        active_idx = step if step < 10 else (18 - step)
        
        # Smooth 3-dot comet sweep
        beads = [False] * 10
        lead_bead = active_idx
        for b in range(10):
            if b == lead_bead or b == lead_bead - 1 or b == lead_bead + 1:
                beads[b] = True
                
        # Colon blinks once per second
        colon_on = (f < total_frames // 2)
        
        frame_img = render_emery_screen(
            d1=2, d2=1, d3=2, d4=5, colon=colon_on,
            footer_img=FOOTER_CYLON,
            beads_lit=beads,
            charging_dot=True
        )
        frames.append(frame_img)
        
    out_path = os.path.join(SCREENSHOTS_DIR, "emery-charging-cylon.gif")
    frames[0].save(out_path, save_all=True, append_images=frames[1:], duration=int(1000/fps), loop=0)
    frames[15].save(os.path.join(SCREENSHOTS_DIR, "emery-charging-cylon.png"))
    print(f"  [✓] Saved {out_path} (60 frames @ 30 FPS)", flush=True)

def render_marquee_animation():
    """Generates ultra-smooth 20 FPS 1970s Theater Marquee GIF."""
    print("Generating High-Fidelity 20 FPS Theater Marquee GIF...", flush=True)
    fps = 20
    duration_sec = 2.0
    total_frames = int(fps * duration_sec) # 40 frames
    frames = []
    
    for f in range(total_frames):
        phase = ((f % 20) < 10) # alternates every 500ms
        beads = [((i % 2 == 0) == phase) for i in range(10)]
        colon_on = (f < total_frames // 2)
        
        frame_img = render_emery_screen(
            d1=2, d2=1, d3=2, d4=6, colon=colon_on,
            footer_img=FOOTER_MARQUEE,
            beads_lit=beads,
            charging_dot=True
        )
        frames.append(frame_img)
        
    out_path = os.path.join(SCREENSHOTS_DIR, "emery-charging-marquee.gif")
    frames[0].save(out_path, save_all=True, append_images=frames[1:], duration=int(1000/fps), loop=0)
    frames[5].save(os.path.join(SCREENSHOTS_DIR, "emery-charging-marquee.png"))
    print(f"  [✓] Saved {out_path} (40 frames @ 20 FPS)", flush=True)

def render_overdrive_animation():
    """Generates ultra-smooth 20 FPS Step Overdrive Celebration GIF."""
    print("Generating High-Fidelity 20 FPS Step Overdrive GIF...", flush=True)
    fps = 20
    duration_sec = 2.0
    total_frames = int(fps * duration_sec) # 40 frames
    frames = []
    
    for f in range(total_frames):
        # Sine wave breathing pulse for lap 2 beads
        pulse_phase = math.sin(f * (2 * math.pi / 20)) # 1.0s cycle
        pulse_intensity = 0.2 + 0.8 * max(0.0, pulse_phase)
        
        # 12,500 steps = base 10 beads lit + 2 surplus beads pulsing
        beads = [True] * 10
        
        # Render 1 2 5 0 0
        frame_img = render_emery_screen(
            d1=1, d2=2, d3=5, d4=0, d5=0,
            is_5_digit=True,
            footer_img=FOOTER_OVERDRIVE,
            beads_lit=beads,
            bead_pulse_idx=2, # Pulse first 3 beads (Lap 2)
            bead_pulse_intensity=pulse_intensity
        )
        frames.append(frame_img)
        
    out_path = os.path.join(SCREENSHOTS_DIR, "emery-steps-overdrive.gif")
    frames[0].save(out_path, save_all=True, append_images=frames[1:], duration=int(1000/fps), loop=0)
    print(f"  [✓] Saved {out_path} (40 frames @ 20 FPS)", flush=True)

def build_hardware_mockup_frame(screen_img, tap_ripple_radius=0):
    """Embeds a 200x228 screen into a high-resolution Apple/Pebble Time 2 hardware bezel frame with optional tap highlight."""
    canvas_w, canvas_h = 320, 380
    canvas = Image.new("RGBA", (canvas_w, canvas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(canvas)
    
    # Outer strap
    strap_w = 180
    draw.rounded_rectangle([(canvas_w - strap_w)//2, 10, (canvas_w + strap_w)//2, canvas_h - 10], radius=12, fill=(28, 30, 33, 255))
    for y in range(20, 70, 12):
        draw.line([(canvas_w - strap_w + 30)//2, y, (canvas_w + strap_w - 30)//2, y], fill=(42, 45, 49, 255), width=2)
    for y in range(canvas_h - 70, canvas_h - 20, 12):
        draw.line([(canvas_w - strap_w + 30)//2, y, (canvas_w + strap_w - 30)//2, y], fill=(42, 45, 49, 255), width=2)
        
    # Stainless steel casing
    case_w, case_h = 248, 280
    case_x = (canvas_w - case_w) // 2
    case_y = (canvas_h - case_h) // 2
    draw.rounded_rectangle([case_x, case_y, case_x + case_w, case_y + case_h], radius=32, fill=(215, 220, 225, 255), outline=(175, 180, 185, 255), width=3)
    
    # Bezel
    bezel_w, bezel_h = 224, 254
    bezel_x = (canvas_w - bezel_w) // 2
    bezel_y = (canvas_h - bezel_h) // 2
    draw.rounded_rectangle([bezel_x, bezel_y, bezel_x + bezel_w, bezel_y + bezel_h], radius=20, fill=(18, 18, 20, 255), outline=(40, 42, 46, 255), width=2)
    
    # Screen paste
    sx = (canvas_w - 200) // 2
    sy = (canvas_h - 228) // 2
    canvas.paste(screen_img, (sx, sy))
    
    # Interactive Tap Ripple / Gesture Effect
    if tap_ripple_radius > 0:
        center_x = canvas_w // 2
        center_y = canvas_h // 2
        r = tap_ripple_radius
        alpha = max(0, int(220 * (1.0 - r / 100.0)))
        draw.ellipse([center_x - r, center_y - r, center_x + r, center_y + r], outline=(255, 60, 60, alpha), width=3)
        
    return canvas

def render_hero_flick_actions_gif():
    """Generates the master showcase hero GIF with live ticking seconds, beating heart, and dynamic tap interactions."""
    print("Generating Master 20 FPS Showcase Hero GIF (Device Mockup)...", flush=True)
    fps = 20
    frames = []
    
    def add_mode_frames(render_fn, duration_sec, tap_transition=True):
        count = int(duration_sec * fps)
        for i in range(count):
            t_sec = i / fps
            screen = render_fn(t_sec, i)
            # Add tap ripple at transition start
            ripple_r = 0
            if tap_transition and i < 8:
                ripple_r = int(i * 12)
            mockup = build_hardware_mockup_frame(screen, tap_ripple_radius=ripple_r)
            frames.append(mockup)
            
    # 1. TIME MODE (21:24, colon blinks every second)
    def render_time(t, f):
        colon = (int(t * 2) % 2 == 0)
        beads = [True]*8 + [False]*2
        return render_emery_screen(d1=2, d2=1, d3=2, d4=4, colon=colon, footer_img=FOOTER_TIME, beads_lit=beads)
    add_mode_frames(render_time, duration_sec=1.6, tap_transition=False)
    
    # 2. LIVE SECONDS MODE (:07, :08, :09, :10 ticking!)
    def render_seconds(t, f):
        sec_val = 7 + int(t * 1.5)
        tens = sec_val // 10
        ones = sec_val % 10
        return render_emery_screen(d1=10, d2=10, d3=tens, d4=ones, colon=True, footer_img=FOOTER_SECONDS)
    add_mode_frames(render_seconds, duration_sec=2.0, tap_transition=True)
    
    # 3. DATE MODE (08.22)
    def render_date(t, f):
        return render_emery_screen(d1=0, d2=8, d3=2, d4=2, colon=True, footer_img=FOOTER_DATE)
    add_mode_frames(render_date, duration_sec=1.5, tap_transition=True)
    
    # 4. STEPS MODE (08420 with 8 active step beads)
    def render_steps(t, f):
        beads = [True]*8 + [False]*2
        return render_emery_screen(d1=0, d2=8, d3=4, d4=2, d5=0, is_5_digit=True, footer_img=FOOTER_STEPS, beads_lit=beads)
    add_mode_frames(render_steps, duration_sec=1.5, tap_transition=True)
    
    # 5. BATTERY MODE ( 85%)
    def render_battery(t, f):
        beads = [True]*8 + [False]*2
        return render_emery_screen(d1=10, d2=8, d3=5, d4=14, footer_img=FOOTER_BATTERY, beads_lit=beads)
    add_mode_frames(render_battery, duration_sec=1.5, tap_transition=True)
    
    # 6. HEART RATE MODE (♥  72 with live beating heart glyph!)
    def render_heart(t, f):
        # 72 BPM = 1 beat every 0.833s (dual lub-dub pulse)
        beat_t = (t % 0.833) / 0.833
        intensity = 1.0
        if beat_t < 0.15 or (beat_t >= 0.25 and beat_t < 0.40):
            intensity = 1.4 # Heart pulses brightly!
        else:
            intensity = 0.8
        return render_emery_screen(d1=18, d2=10, d3=10, d4=7, d5=2, is_5_digit=True, footer_img=FOOTER_HEART, heart_intensity=intensity)
    add_mode_frames(render_heart, duration_sec=2.2, tap_transition=True)
    
    out_path = os.path.join(RENDERS_DIR, "pebble-pulsar-flick-actions-optimized.gif")
    frames[0].save(out_path, save_all=True, append_images=frames[1:], duration=int(1000/fps), loop=0)
    print(f"  [✓] Saved {out_path} ({len(frames)} frames @ 20 FPS)", flush=True)

def main():
    print("==========================================", flush=True)
    print("Rendering High-Fidelity Animated GIFs", flush=True)
    print("==========================================", flush=True)
    
    render_cylon_animation()
    render_marquee_animation()
    render_overdrive_animation()
    render_hero_flick_actions_gif()
    
    print("\n✓ All High-Fidelity GIFs Successfully Generated!", flush=True)

if __name__ == "__main__":
    main()
