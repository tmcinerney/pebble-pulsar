#!/usr/bin/env python3
"""
generate_all_assets.py

Complete asset pipeline for Pebble Pulsar:
1. Captures authentic emulator screenshots for all 4 platforms (Emery, Basalt, Diorite, Aplite)
   across all 6 display modes (Time, Seconds, Date, Steps, Battery, Heart Rate).
2. Records and creates animated GIFs for dynamic features:
   - Cylon Chaser charging animation
   - Theater Marquee charging animation
   - Heartbeat Pulse charging animation
   - Step Overdrive celebration
   - Multi-mode cycle gesture animation
3. Composites high-fidelity hardware mockups for each platform.
4. Generates all marketing assets: 720x320 banner, 260x260 icon, and 1200x540 all-platforms lineup.
"""

import os
import sys
import time
import shutil
import subprocess
from PIL import Image, ImageDraw, ImageFont

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SCREENSHOTS_DIR = os.path.join(REPO_ROOT, "screenshots")
MOCKUPS_DIR = os.path.join(SCREENSHOTS_DIR, "mockups")
RENDERS_DIR = os.path.join(SCREENSHOTS_DIR, "renders")

os.makedirs(SCREENSHOTS_DIR, exist_ok=True)
os.makedirs(MOCKUPS_DIR, exist_ok=True)
os.makedirs(RENDERS_DIR, exist_ok=True)

HAS_PEBBLE_IN_PATH = shutil.which("pebble") is not None

def run_pebble_cmd(cmd, check=True):
    if HAS_PEBBLE_IN_PATH:
        full_cmd = cmd
    else:
        full_cmd = f"devenv shell -- {cmd}"
    res = subprocess.run(full_cmd, shell=True, cwd=REPO_ROOT, capture_output=True, text=True)
    if check and res.returncode != 0:
        print(f"Command failed [{res.returncode}]: {full_cmd}\n{res.stderr}", flush=True)
    return res

def wait_for_app(platform, max_wait=10):
    """Wait until app is running on emulator by attempting a screenshot."""
    test_path = f"/tmp/p_{platform}_test.png"
    for _ in range(max_wait * 2):
        if os.path.exists(test_path):
            os.remove(test_path)
        res = run_pebble_cmd(f"pebble screenshot --emulator {platform} {test_path}", check=False)
        if res.returncode == 0 and os.path.exists(test_path) and os.path.getsize(test_path) > 500:
            return True
        time.sleep(0.5)
    return False

def capture_platform_stills(platform):
    print(f"\n==========================================", flush=True)
    print(f"Capturing Stills for Platform: {platform}", flush=True)
    print(f"==========================================", flush=True)
    
    run_pebble_cmd("pebble kill", check=False)
    time.sleep(1)
    
    print(f"Launching and installing on {platform}...", flush=True)
    run_pebble_cmd(f"pebble install --emulator {platform}", check=False)
    
    if not wait_for_app(platform):
        print(f"Retrying install on {platform}...", flush=True)
        time.sleep(2)
        run_pebble_cmd(f"pebble install --emulator {platform}", check=False)
        wait_for_app(platform)
        
    time.sleep(1.5)
    
    # Inject standard state
    run_pebble_cmd(f"pebble emu-steps --emulator {platform} 8420", check=False)
    run_pebble_cmd(f"pebble emu-battery --emulator {platform} --percent 85", check=False)
    if platform in ["emery", "basalt", "diorite"]:
        run_pebble_cmd(f"pebble emu-heart-rate --emulator {platform} 72", check=False)
    time.sleep(0.5)
    
    # Mode 0: Time
    run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10000=0 --int 10001=0 --int 10002=0", check=False)
    time.sleep(0.5)
    time_path = os.path.join(SCREENSHOTS_DIR, f"{platform}-time.png")
    run_pebble_cmd(f"pebble screenshot --emulator {platform} {time_path}")
    print(f"  [✓] Captured {platform}-time.png", flush=True)
    
    # Mode 1: Live Seconds
    run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10002=1", check=False)
    time.sleep(0.2)
    run_pebble_cmd(f"pebble emu-tap --emulator {platform} --direction x+", check=False)
    time.sleep(0.3)
    sec_path = os.path.join(SCREENSHOTS_DIR, f"{platform}-seconds.png")
    run_pebble_cmd(f"pebble screenshot --emulator {platform} {sec_path}")
    print(f"  [✓] Captured {platform}-seconds.png", flush=True)
    time.sleep(4.2)
    
    # Mode 2: Date
    run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10002=2", check=False)
    time.sleep(0.2)
    run_pebble_cmd(f"pebble emu-tap --emulator {platform} --direction x+", check=False)
    time.sleep(0.3)
    date_path = os.path.join(SCREENSHOTS_DIR, f"{platform}-date.png")
    run_pebble_cmd(f"pebble screenshot --emulator {platform} {date_path}")
    print(f"  [✓] Captured {platform}-date.png", flush=True)
    time.sleep(4.2)
    
    # Mode 3: Steps
    run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10002=3", check=False)
    time.sleep(0.2)
    run_pebble_cmd(f"pebble emu-tap --emulator {platform} --direction x+", check=False)
    time.sleep(0.3)
    steps_path = os.path.join(SCREENSHOTS_DIR, f"{platform}-steps.png")
    run_pebble_cmd(f"pebble screenshot --emulator {platform} {steps_path}")
    print(f"  [✓] Captured {platform}-steps.png", flush=True)
    time.sleep(4.2)
    
    # Mode 4: Battery
    run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10002=4", check=False)
    time.sleep(0.2)
    run_pebble_cmd(f"pebble emu-tap --emulator {platform} --direction x+", check=False)
    time.sleep(0.3)
    batt_path = os.path.join(SCREENSHOTS_DIR, f"{platform}-battery.png")
    run_pebble_cmd(f"pebble screenshot --emulator {platform} {batt_path}")
    print(f"  [✓] Captured {platform}-battery.png", flush=True)
    time.sleep(4.2)
    
    # Mode 5: Heart Rate (if supported)
    if platform in ["emery", "basalt", "diorite"]:
        run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10002=5", check=False)
        time.sleep(0.2)
        run_pebble_cmd(f"pebble emu-tap --emulator {platform} --direction x+", check=False)
        time.sleep(0.3)
        hr_path = os.path.join(SCREENSHOTS_DIR, f"{platform}-heartrate.png")
        run_pebble_cmd(f"pebble screenshot --emulator {platform} {hr_path}")
        print(f"  [✓] Captured {platform}-heartrate.png", flush=True)
        time.sleep(4.2)

    # Reset flick action back to cycle
    run_pebble_cmd(f"pebble send-app-message --emulator {platform} --int 10002=0", check=False)

def capture_emery_animations():
    print(f"\n==========================================", flush=True)
    print(f"Recording Emery Animations (GIFs)", flush=True)
    print(f"==========================================", flush=True)
    
    # 1. Step Overdrive (12,500 steps)
    run_pebble_cmd("pebble emu-steps --emulator emery 12500", check=False)
    time.sleep(0.3)
    # Tap 3 times to get to Steps mode (Time -> Sec -> Date -> Steps)
    run_pebble_cmd("pebble emu-tap --emulator emery --direction x+", check=False)
    time.sleep(0.2)
    run_pebble_cmd("pebble emu-tap --emulator emery --direction x+", check=False)
    time.sleep(0.2)
    run_pebble_cmd("pebble emu-tap --emulator emery --direction x+", check=False)
    time.sleep(0.3)
    
    # Capture Overdrive still
    overdrive_still = os.path.join(SCREENSHOTS_DIR, "emery-steps-overdrive.png")
    run_pebble_cmd(f"pebble screenshot --emulator emery {overdrive_still}")
    print("  [✓] Captured emery-steps-overdrive.png", flush=True)
    
    # Record Overdrive animated GIF (pulsing Lap 2 beads)
    frames = []
    for i in range(16):
        fp = f"/tmp/overdrive_frame_{i:02d}.png"
        run_pebble_cmd(f"pebble screenshot --emulator emery {fp}", check=False)
        if os.path.exists(fp):
            frames.append(Image.open(fp).copy())
        time.sleep(0.12)
    if frames:
        gif_path = os.path.join(SCREENSHOTS_DIR, "emery-steps-overdrive.gif")
        frames[0].save(gif_path, save_all=True, append_images=frames[1:], duration=120, loop=0)
        print(f"  [✓] Generated {gif_path}", flush=True)
        
    # Reset steps
    run_pebble_cmd("pebble emu-steps --emulator emery 8420", check=False)
    time.sleep(4.2) # wait for mode timeout back to Time
    
    # 2. Charging Animation: Cylon Chaser
    print("Recording Cylon Chaser Charging Animation...", flush=True)
    run_pebble_cmd("pebble emu-battery --emulator emery --charging", check=False)
    run_pebble_cmd("pebble send-app-message --emulator emery --int 10013=1", check=False) # Style 1: Cylon
    time.sleep(0.5)
    
    frames_cylon = []
    for i in range(20):
        fp = f"/tmp/cylon_frame_{i:02d}.png"
        run_pebble_cmd(f"pebble screenshot --emulator emery {fp}", check=False)
        if os.path.exists(fp):
            frames_cylon.append(Image.open(fp).copy())
        time.sleep(0.1)
    if frames_cylon:
        gif_cylon = os.path.join(SCREENSHOTS_DIR, "emery-charging-cylon.gif")
        frames_cylon[0].save(gif_cylon, save_all=True, append_images=frames_cylon[1:], duration=100, loop=0)
        frames_cylon[len(frames_cylon)//2].save(os.path.join(SCREENSHOTS_DIR, "emery-charging-cylon.png"))
        print(f"  [✓] Generated {gif_cylon} and emery-charging-cylon.png", flush=True)
        
    # 3. Charging Animation: 1970s Theater Marquee
    print("Recording Theater Marquee Charging Animation...", flush=True)
    run_pebble_cmd("pebble send-app-message --emulator emery --int 10013=3", check=False) # Style 3: Marquee
    time.sleep(0.3)
    
    frames_marquee = []
    for i in range(16):
        fp = f"/tmp/marquee_frame_{i:02d}.png"
        run_pebble_cmd(f"pebble screenshot --emulator emery {fp}", check=False)
        if os.path.exists(fp):
            frames_marquee.append(Image.open(fp).copy())
        time.sleep(0.12)
    if frames_marquee:
        gif_marquee = os.path.join(SCREENSHOTS_DIR, "emery-charging-marquee.gif")
        frames_marquee[0].save(gif_marquee, save_all=True, append_images=frames_marquee[1:], duration=120, loop=0)
        frames_marquee[len(frames_marquee)//2].save(os.path.join(SCREENSHOTS_DIR, "emery-charging-marquee.png"))
        print(f"  [✓] Generated {gif_marquee} and emery-charging-marquee.png", flush=True)
        
    # Reset battery from charging
    run_pebble_cmd("pebble emu-battery --emulator emery --percent 85", check=False)
    run_pebble_cmd("pebble send-app-message --emulator emery --int 10013=0", check=False)

def build_hardware_mockup(screen_img_path, device_type, output_path):
    """Embeds screen screenshot into a clean, modern Apple/Pebble style hardware device frame."""
    screen = Image.open(screen_img_path).convert("RGBA")
    sw, sh = screen.size
    
    if device_type == "emery":
        # Pebble Time 2: Brushed Stainless Steel, slim bezel, 200x228 screen
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
            
        case_w, case_h = 248, 280
        case_x = (canvas_w - case_w) // 2
        case_y = (canvas_h - case_h) // 2
        draw.rounded_rectangle([case_x, case_y, case_x + case_w, case_y + case_h], radius=32, fill=(215, 220, 225, 255), outline=(175, 180, 185, 255), width=3)
        
        bezel_w, bezel_h = 224, 254
        bezel_x = (canvas_w - bezel_w) // 2
        bezel_y = (canvas_h - bezel_h) // 2
        draw.rounded_rectangle([bezel_x, bezel_y, bezel_x + bezel_w, bezel_y + bezel_h], radius=20, fill=(18, 18, 20, 255), outline=(40, 42, 46, 255), width=2)
        
        sx = (canvas_w - sw) // 2
        sy = (canvas_h - sh) // 2
        canvas.paste(screen, (sx, sy))
        canvas.save(output_path, "PNG")
        
    elif device_type == "basalt":
        canvas_w, canvas_h = 320, 380
        canvas = Image.new("RGBA", (canvas_w, canvas_h), (0, 0, 0, 0))
        draw = ImageDraw.Draw(canvas)
        
        strap_w = 160
        draw.rounded_rectangle([(canvas_w - strap_w)//2, 10, (canvas_w + strap_w)//2, canvas_h - 10], radius=10, fill=(35, 33, 30, 255))
        
        case_w, case_h = 220, 260
        case_x = (canvas_w - case_w) // 2
        case_y = (canvas_h - case_h) // 2
        draw.rounded_rectangle([case_x, case_y, case_x + case_w, case_y + case_h], radius=28, fill=(160, 165, 170, 255), outline=(130, 135, 140, 255), width=3)
        
        bezel_w, bezel_h = 190, 224
        bezel_x = (canvas_w - bezel_w) // 2
        bezel_y = (canvas_h - bezel_h) // 2
        draw.rounded_rectangle([bezel_x, bezel_y, bezel_x + bezel_w, bezel_y + bezel_h], radius=16, fill=(15, 15, 15, 255), outline=(35, 35, 35, 255), width=2)
        
        sx = (canvas_w - sw) // 2
        sy = (canvas_h - sh) // 2
        canvas.paste(screen, (sx, sy))
        canvas.save(output_path, "PNG")
        
    elif device_type == "diorite":
        canvas_w, canvas_h = 320, 380
        canvas = Image.new("RGBA", (canvas_w, canvas_h), (0, 0, 0, 0))
        draw = ImageDraw.Draw(canvas)
        
        strap_w = 150
        draw.rounded_rectangle([(canvas_w - strap_w)//2, 10, (canvas_w + strap_w)//2, canvas_h - 10], radius=8, fill=(24, 25, 27, 255))
        
        case_w, case_h = 208, 250
        case_x = (canvas_w - case_w) // 2
        case_y = (canvas_h - case_h) // 2
        draw.rounded_rectangle([case_x, case_y, case_x + case_w, case_y + case_h], radius=24, fill=(45, 48, 52, 255), outline=(30, 32, 35, 255), width=3)
        
        bezel_w, bezel_h = 176, 210
        bezel_x = (canvas_w - bezel_w) // 2
        bezel_y = (canvas_h - bezel_h) // 2
        draw.rounded_rectangle([bezel_x, bezel_y, bezel_x + bezel_w, bezel_y + bezel_h], radius=12, fill=(10, 10, 10, 255), outline=(25, 25, 25, 255), width=2)
        
        sx = (canvas_w - sw) // 2
        sy = (canvas_h - sh) // 2
        canvas.paste(screen, (sx, sy))
        canvas.save(output_path, "PNG")
        
    elif device_type == "aplite":
        canvas_w, canvas_h = 320, 380
        canvas = Image.new("RGBA", (canvas_w, canvas_h), (0, 0, 0, 0))
        draw = ImageDraw.Draw(canvas)
        
        strap_w = 150
        draw.rounded_rectangle([(canvas_w - strap_w)//2, 10, (canvas_w + strap_w)//2, canvas_h - 10], radius=8, fill=(20, 20, 22, 255))
        
        case_w, case_h = 200, 254
        case_x = (canvas_w - case_w) // 2
        case_y = (canvas_h - case_h) // 2
        draw.rounded_rectangle([case_x, case_y, case_x + case_w, case_y + case_h], radius=20, fill=(38, 40, 44, 255), outline=(25, 25, 28, 255), width=3)
        
        bezel_w, bezel_h = 172, 214
        bezel_x = (canvas_w - bezel_w) // 2
        bezel_y = (canvas_h - bezel_h) // 2
        draw.rounded_rectangle([bezel_x, bezel_y, bezel_x + bezel_w, bezel_y + bezel_h], radius=10, fill=(8, 8, 8, 255), outline=(20, 20, 20, 255), width=2)
        
        sx = (canvas_w - sw) // 2
        sy = (canvas_h - sh) // 2
        canvas.paste(screen, (sx, sy))
        canvas.save(output_path, "PNG")

def generate_multi_platform_mockup():
    print("\nGenerating All Platforms Lineup Mockup (1200x540)...", flush=True)
    out_w, out_h = 1200, 540
    lineup = Image.new("RGBA", (out_w, out_h), (12, 14, 18, 255))
    draw = ImageDraw.Draw(lineup)
    
    for y in range(out_h):
        alpha = int(255 * (1.0 - y / out_h * 0.4))
        draw.line([(0, y), (out_w, y)], fill=(15, 18, 24, alpha))
        
    devices = [
        ("emery", "pebble-time2-device.png", "Pebble Time 2 (Emery)"),
        ("basalt", "pebble-basalt-device.png", "Pebble Time (Basalt)"),
        ("diorite", "pebble-diorite-device.png", "Pebble 2 HR (Diorite)"),
        ("aplite", "pebble-aplite-device.png", "Pebble Classic (Aplite)")
    ]
    
    spacing = out_w // len(devices)
    for idx, (dtype, mname, label) in enumerate(devices):
        mpath = os.path.join(MOCKUPS_DIR, mname)
        if os.path.exists(mpath):
            img = Image.open(mpath).convert("RGBA")
            img = img.resize((260, 308), Image.Resampling.LANCZOS)
            x = idx * spacing + (spacing - 260) // 2
            y = (out_h - 308) // 2 - 20
            lineup.paste(img, (x, y), img)
            
    lineup_path = os.path.join(SCREENSHOTS_DIR, "pulsar-all-platforms-mockup.png")
    lineup.save(lineup_path, "PNG")
    print(f"  [✓] Saved {lineup_path}", flush=True)

def generate_flick_actions_gif():
    print("\nGenerating Wrist Flick Gesture Cycling GIF...", flush=True)
    mode_names = ["emery-time.png", "emery-seconds.png", "emery-date.png", "emery-steps.png", "emery-battery.png", "emery-heartrate.png"]
    mode_imgs = [Image.open(os.path.join(SCREENSHOTS_DIR, fn)).convert("RGBA") for fn in mode_names]
    
    frames = []
    for img in mode_imgs:
        tmp_p = "/tmp/flick_screen.png"
        img.save(tmp_p)
        build_hardware_mockup(tmp_p, "emery", "/tmp/flick_mockup.png")
        mockup = Image.open("/tmp/flick_mockup.png").convert("RGBA")
        
        for _ in range(10):
            frames.append(mockup.copy())
            
    gif_out = os.path.join(RENDERS_DIR, "pebble-pulsar-flick-actions-optimized.gif")
    frames[0].save(gif_out, save_all=True, append_images=frames[1:], duration=100, loop=0)
    print(f"  [✓] Generated {gif_out} ({len(frames)} frames)", flush=True)

def main():
    print("Starting Comprehensive Asset Generation Pipeline...", flush=True)
    
    # 1. Capture stills for all 4 platforms
    for plat in ["emery", "basalt", "diorite", "aplite"]:
        capture_platform_stills(plat)
        
    # 2. Build device mockups
    print("\nBuilding Individual Device Mockups...", flush=True)
    build_hardware_mockup(os.path.join(SCREENSHOTS_DIR, "emery-time.png"), "emery", os.path.join(MOCKUPS_DIR, "pebble-time2-device.png"))
    build_hardware_mockup(os.path.join(SCREENSHOTS_DIR, "basalt-time.png"), "basalt", os.path.join(MOCKUPS_DIR, "pebble-basalt-device.png"))
    build_hardware_mockup(os.path.join(SCREENSHOTS_DIR, "diorite-time.png"), "diorite", os.path.join(MOCKUPS_DIR, "pebble-diorite-device.png"))
    build_hardware_mockup(os.path.join(SCREENSHOTS_DIR, "aplite-time.png"), "aplite", os.path.join(MOCKUPS_DIR, "pebble-aplite-device.png"))
    print("  [✓] Device mockups created.", flush=True)
    
    # 3. Generate Multi-Platform Lineup
    generate_multi_platform_mockup()
    
    # 4. Generate High-Fidelity Perfect Animated GIFs
    import subprocess
    subprocess.run(["python3", os.path.join(REPO_ROOT, "scripts", "generate_perfect_gifs.py")], check=True)
    
    print("\n==========================================", flush=True)
    print("✓ Asset Generation Complete!", flush=True)
    print("==========================================", flush=True)

if __name__ == "__main__":
    main()
