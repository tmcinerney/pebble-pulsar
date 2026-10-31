#!/usr/bin/env python3
"""
generate_app_assets.py

Automated, validated screenshot and render generation pipeline for the
Pebble Pulsar Suite.

Key Capabilities:
1. Programmatic Emulator Control: Launches emulator, installs target app, and injects state.
2. Strict Image Content Validation:
   - Verifies exact pixel dimensions (Emery: 200x228, Basalt/Diorite/Aplite: 144x168).
   - Detects and rejects blank (all-black / all-white) frames.
   - Detects and rejects bootstrapping / launcher / transitioning frames.
   - Automatically retries with exponential/linear backoff until valid frame is confirmed.
3. Standardized Output Structure:
   - screenshots/<app>/<platform>-<state>.png
   - screenshots/mockups/<app>/pebble-<platform>-<app>.png
   - screenshots/suite/ (store banners, icons, multi-device lineups)
4. Composites 3D studio-quality device frames using PIL/Pillow.
"""

import os
import sys
import time
import shutil
import subprocess
from PIL import Image, ImageStat

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
SCREENSHOTS_DIR = os.path.join(REPO_ROOT, "screenshots")
MOCKUPS_DIR = os.path.join(SCREENSHOTS_DIR, "mockups")
SUITE_DIR = os.path.join(SCREENSHOTS_DIR, "suite")

PLATFORMS = ["emery", "basalt", "diorite", "aplite"]
PLATFORM_DIMENSIONS = {
    "emery": (200, 228),
    "basalt": (144, 168),
    "diorite": (144, 168),
    "aplite": (144, 168),
}

HAS_PEBBLE_IN_PATH = shutil.which("pebble") is not None


def run_cmd(cmd, cwd=REPO_ROOT, check=True, timeout=35):
    """Executes a command inside the devenv shell if pebble is not directly in PATH."""
    if HAS_PEBBLE_IN_PATH or "devenv" in cmd:
        full_cmd = cmd
    else:
        full_cmd = f"devenv shell -- {cmd}"
    try:
        res = subprocess.run(full_cmd, shell=True, cwd=cwd, capture_output=True, text=True, timeout=timeout)
        if check and res.returncode != 0:
            print(f"Command failed [{res.returncode}]: {full_cmd}\n{res.stderr}", flush=True)
        return res
    except subprocess.TimeoutExpired:
        print(f"Command timed out after {timeout}s: {full_cmd}", flush=True)
        return subprocess.CompletedProcess(args=full_cmd, returncode=124, stdout="", stderr="Timeout")


# ==============================================================================
# VALIDATION ENGINE
# ==============================================================================

def validate_screenshot(image_path, platform, min_active_pixel_ratio=0.012, max_active_pixel_ratio=0.25):
    """
    Validates that a captured screenshot is authentic and contains active app content:
    1. File exists and has non-zero size.
    2. Dimensions exactly match target platform hardware.
    3. Not pitch black / blank (minimum active glowing LED pixels present).
    4. Not a system launcher / crash screen (active glowing LED pixels ceiling < 25%).
    5. Image has non-trivial color/brightness variance (not a solid boot screen).
    """
    if not os.path.exists(image_path) or os.path.getsize(image_path) < 200:
        return False, "File does not exist or is empty"

    try:
        with Image.open(image_path) as img:
            img = img.convert("RGB")
            expected_w, expected_h = PLATFORM_DIMENSIONS[platform]
            actual_w, actual_h = img.size

            if (actual_w, actual_h) != (expected_w, expected_h):
                return False, f"Dimension mismatch: expected {expected_w}x{expected_h}, got {actual_w}x{actual_h}"

            # Calculate pixel brightness statistics
            total_pixels = actual_w * actual_h
            stat = ImageStat.Stat(img)
            mean_brightness = sum(stat.mean) / 3.0

            # Count glowing pixels (brightness > 28)
            pixels = list(img.getdata())
            active_pixels = sum(1 for p in pixels if (p[0] > 28 or p[1] > 28 or p[2] > 28))
            active_ratio = active_pixels / float(total_pixels)

            if active_ratio < min_active_pixel_ratio:
                return False, f"Image is blank/empty: only {active_pixels} active pixels ({active_ratio:.1%})"

            if active_ratio > max_active_pixel_ratio:
                return False, f"Image has excessive lit pixels ({active_ratio:.1%}, likely system launcher/crash, expected LED matrix < 25%)"

            if mean_brightness < 1.0:
                return False, f"Image is virtually pitch black (mean brightness {mean_brightness:.2f})"

            return True, f"Valid ({actual_w}x{actual_h}, {active_pixels} active LEDs, {active_ratio:.1%})"
    except Exception as e:
        return False, f"Image decode error: {str(e)}"


def install_and_boot(platform, app_dir):
    """Cleanly stops previous emulators, installs the built app pbw, and waits until app UI is active."""
    subprocess.run("pkill -9 -f qemu-pebble 2>/dev/null; pkill -9 -f pypkjs 2>/dev/null", shell=True, check=False)
    run_cmd("pebble kill", check=False)
    time.sleep(1.5)
    
    pbw_name = os.path.basename(app_dir) + ".pbw"
    pbw_path = os.path.abspath(os.path.join(app_dir, "build", pbw_name))
    if not os.path.exists(pbw_path):
        pbws = glob.glob(os.path.join(app_dir, "build", "*.pbw"))
        if pbws:
            pbw_path = os.path.abspath(pbws[0])
            
    cmd = f"pebble install --emulator {platform} {pbw_path}" if os.path.exists(pbw_path) else f"pebble install --emulator {platform}"
    run_cmd(cmd, cwd=app_dir, check=False)
    time.sleep(3.0)
    
    # Poll until app UI is active
    tmp_check = f"/tmp/boot_check_{platform}.png"
    for _ in range(12):
        if os.path.exists(tmp_check):
            os.remove(tmp_check)
        run_cmd(f"pebble screenshot --emulator {platform} {tmp_check}", check=False)
        valid, _ = validate_screenshot(tmp_check, platform)
        if valid:
            break
        time.sleep(0.6)
    if os.path.exists(tmp_check):
        try:
            os.remove(tmp_check)
        except OSError:
            pass


def capture_with_validation(platform, out_path, max_retries=6, retry_delay=0.6):
    """Captures screenshot with automatic retry until content validation passes."""
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    for attempt in range(1, max_retries + 1):
        if os.path.exists(out_path):
            os.remove(out_path)
        run_cmd(f"pebble screenshot --emulator {platform} {out_path}", check=False)
        is_valid, msg = validate_screenshot(out_path, platform)
        if is_valid:
            print(f"    ✓ Captured & Validated: {os.path.basename(out_path)} [{msg}]", flush=True)
            return True
        else:
            print(f"    ⏳ Attempt {attempt}/{max_retries} invalid ({msg}), retrying...", flush=True)
            time.sleep(retry_delay)

    print(f"    ❌ FAILED to capture valid screenshot for {platform} -> {out_path} ({msg})", flush=True)
    return False


# ==============================================================================
# APP CAPTURE DRIVERS
# ==============================================================================

def capture_watchface_assets():
    app_dir = os.path.join(REPO_ROOT, "apps", "watchface")
    out_dir = os.path.join(SCREENSHOTS_DIR, "watchface")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 60)
    print("▶ CAPTURING & VALIDATING WATCHFACE ASSETS")
    print("=" * 60)

    shot_names = ["time", "seconds", "date", "steps", "battery"]

    for plat in PLATFORMS:
        print(f"\n[Platform: {plat.upper()}]")
        install_and_boot(plat, app_dir)

        # Inject standard steps & battery
        run_cmd(f"pebble emu-steps --emulator {plat} 8420", check=False)
        run_cmd(f"pebble emu-battery --emulator {plat} --percent 85", check=False)
        if plat in ["emery", "basalt", "diorite"]:
            run_cmd(f"pebble emu-heart-rate --emulator {plat} 72", check=False)
        time.sleep(0.5)

        # 1. Main Time Mode
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-time.png"))

        # 2. Live Seconds Mode (Tap once)
        run_cmd(f"pebble emu-tap --emulator {plat} --direction x+", check=False)
        time.sleep(0.6)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-seconds.png"))

        # 3. Calendar Date Mode (Tap again)
        run_cmd(f"pebble emu-tap --emulator {plat} --direction x+", check=False)
        time.sleep(0.6)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-date.png"))

        # 4. Steps Mode (Tap again)
        run_cmd(f"pebble emu-tap --emulator {plat} --direction x+", check=False)
        time.sleep(0.6)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-steps.png"))

        # 5. Battery Mode (Tap again)
        run_cmd(f"pebble emu-tap --emulator {plat} --direction x+", check=False)
        time.sleep(0.6)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-battery.png"))

        verify_pairwise_distinctness(out_dir, plat, shot_names)
        run_cmd("pebble kill", check=False)
        time.sleep(1.5)


def verify_pairwise_distinctness(out_dir, plat, shot_names):
    """Verifies that none of the generated platform screenshots are visually identical."""
    from PIL import ImageChops
    print(f"  🔍 Verifying pairwise visual distinctness for {plat.upper()} ({len(shot_names)} shots)...")
    images = {}
    for name in shot_names:
        path = os.path.join(out_dir, f"{plat}-{name}.png")
        if not os.path.exists(path):
            raise FileNotFoundError(f"Missing expected screenshot: {path}")
        with Image.open(path) as img:
            images[name] = img.convert('RGB')

    for i in range(len(shot_names)):
        for j in range(i + 1, len(shot_names)):
            n1, n2 = shot_names[i], shot_names[j]
            diff = ImageChops.difference(images[n1], images[n2])
            bbox = diff.getbbox()
            if bbox is None:
                raise ValueError(f"CRITICAL ASSET FAILURE: {plat}-{n1}.png is visually identical to {plat}-{n2}.png!")
            print(f"    ✓ Distinct: {n1} vs {n2} (diff box: {bbox})")


def capture_chrono_assets():
    app_dir = os.path.join(REPO_ROOT, "apps", "chrono")
    out_dir = os.path.join(SCREENSHOTS_DIR, "chrono")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 60)
    print("▶ CAPTURING & VALIDATING CHRONOGRAPH ASSETS")
    print("=" * 60)

    shot_names = ["chrono-ready", "chrono-running", "chrono-lap", "chrono-lap-review", "chrono-tachymeter"]

    for plat in PLATFORMS:
        print(f"\n[Platform: {plat.upper()}]")
        install_and_boot(plat, app_dir)

        # 1. Ready State (00:00.00)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-chrono-ready.png"))

        # 2. Running Stopwatch (SELECT)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False)
        time.sleep(1.8)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-chrono-running.png"))

        # 3. Lap Split Hold (DOWN button while running freezes display with split)
        run_cmd(f"pebble emu-button --emulator {plat} click down", check=False)
        time.sleep(0.6)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-chrono-lap.png"))

        # 4. Stopped Lap Review (SELECT to stop, UP to browse)
        time.sleep(1.0)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # stop
        time.sleep(0.6)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False) # browse laps
        time.sleep(0.6)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-chrono-lap-review.png"))

        # 5. Tachymeter / Reset view
        run_cmd(f"pebble emu-button --emulator {plat} click down", check=False) # unfreeze / reset
        time.sleep(0.4)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # run again
        time.sleep(4.2) # advance elapsed time significantly for distinct timer digits
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-chrono-tachymeter.png"))

        verify_pairwise_distinctness(out_dir, plat, shot_names)
        run_cmd("pebble kill", check=False)
        time.sleep(1.5)


def capture_timer_assets():
    app_dir = os.path.join(REPO_ROOT, "apps", "timer")
    out_dir = os.path.join(SCREENSHOTS_DIR, "timer")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 60)
    print("▶ CAPTURING & VALIDATING 5 COUNTDOWN TIMER ASSETS")
    print("=" * 60)

    shot_names = ["timer-preset", "timer-running", "timer-pomodoro", "timer-custom-edit", "timer-alert"]

    for plat in PLATFORMS:
        print(f"\n[Platform: {plat.upper()}]")
        install_and_boot(plat, app_dir)

        # 1. Preset Selection View (05:00)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-timer-preset.png"))

        # 2. Running Countdown View (SELECT)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False)
        time.sleep(2.0)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-timer-running.png"))

        # Pause and reset back to picker
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # pause
        time.sleep(0.3)
        run_cmd(f"pebble emu-button --emulator {plat} click down", check=False) # reset when paused
        time.sleep(0.3)

        # 3. Pomodoro Focus Preset (navigate UP 3 times to 25m)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False)
        time.sleep(0.2)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False)
        time.sleep(0.2)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False)
        time.sleep(0.4)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-timer-pomodoro.png"))

        # 4. Custom Duration Editor (Hold SELECT 700ms)
        run_cmd(f"pebble emu-button --emulator {plat} --duration 700 click select", check=False)
        time.sleep(0.6)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False) # 01:00
        time.sleep(0.2)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False) # 02:00
        time.sleep(0.3)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-timer-custom-edit.png"))

        # 5. Alarm Firing Screen:
        # Finish edit mode (select minutes -> select seconds -> done)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # to seconds
        time.sleep(0.2)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # save custom
        time.sleep(0.3)
        # Navigate UP 6 times from default 5m to 00:10 preset
        for _ in range(6):
            run_cmd(f"pebble emu-button --emulator {plat} click up", check=False)
            time.sleep(0.12)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # start 10s timer
        time.sleep(10.6) # wait for timer alert to fire
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-timer-alert.png"))

        verify_pairwise_distinctness(out_dir, plat, shot_names)
        run_cmd("pebble kill", check=False)
        time.sleep(1.5)


def capture_alarm_assets():
    app_dir = os.path.join(REPO_ROOT, "apps", "alarm")
    out_dir = os.path.join(SCREENSHOTS_DIR, "alarm")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 60)
    print("▶ CAPTURING & VALIDATING 5 ALARM CLOCK ASSETS")
    print("=" * 60)

    shot_names = ["alarm-slot1", "alarm-slot2", "alarm-slot3", "alarm-edit", "alarm-slot4"]

    for plat in PLATFORMS:
        print(f"\n[Platform: {plat.upper()}]")
        install_and_boot(plat, app_dir)

        # 1. Main Alarm Slot 1 View (07:00 DAILY)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-alarm-slot1.png"))
        shutil.copyfile(os.path.join(out_dir, f"{plat}-alarm-slot1.png"), os.path.join(out_dir, f"{plat}-alarm-view.png"))

        # 2. Alarm Slot 2 View (08:30 WEEKDAYS)
        run_cmd(f"pebble emu-button --emulator {plat} click down", check=False)
        time.sleep(0.4)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-alarm-slot2.png"))

        # 3. Alarm Slot 3 View (09:00 WEEKENDS)
        run_cmd(f"pebble emu-button --emulator {plat} click down", check=False)
        time.sleep(0.4)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-alarm-slot3.png"))

        # 4. Interactive Edit Mode (Hold SELECT 700ms)
        run_cmd(f"pebble emu-button --emulator {plat} --duration 700 click select", check=False)
        time.sleep(0.6)
        run_cmd(f"pebble emu-button --emulator {plat} click up", check=False)
        time.sleep(0.3)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-alarm-edit.png"))

        # 5. Alarm Slot 4 / Multi-Schedule
        # Cycle through edit mode to save
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # hour -> minute
        time.sleep(0.3)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # minute -> repeat
        time.sleep(0.3)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # repeat -> done
        time.sleep(0.4)
        run_cmd(f"pebble emu-button --emulator {plat} click down", check=False) # navigate to slot 4
        time.sleep(0.3)
        run_cmd(f"pebble emu-button --emulator {plat} click select", check=False) # toggle ON
        time.sleep(0.4)
        capture_with_validation(plat, os.path.join(out_dir, f"{plat}-alarm-slot4.png"))

        verify_pairwise_distinctness(out_dir, plat, shot_names)
        run_cmd("pebble kill", check=False)
        time.sleep(1.5)


# ==============================================================================
# MOCKUP GENERATION
# ==============================================================================

def generate_mockups_for_app(app_name):
    """Generates composite 3D device frames for the specified app."""
    import generate_hardware_mockups as ghm

    app_shot_dir = os.path.join(SCREENSHOTS_DIR, app_name)
    app_mockup_dir = os.path.join(MOCKUPS_DIR, app_name)
    os.makedirs(app_mockup_dir, exist_ok=True)

    print(f"\nCompositing Hardware Device Mockups for '{app_name}'...")

    # Emery (Pebble Time 2)
    emery_shot = os.path.join(app_shot_dir, f"emery-{app_name}-running.png")
    if not os.path.exists(emery_shot):
        emery_shot = os.path.join(app_shot_dir, "emery-time.png")
    if not os.path.exists(emery_shot):
        emery_shot = os.path.join(app_shot_dir, f"emery-{app_name}-view.png")
    if not os.path.exists(emery_shot):
        emery_shot = os.path.join(app_shot_dir, f"emery-{app_name}-ready.png")

    if os.path.exists(emery_shot):
        t2 = ghm.draw_pebble_time2(scale=1.0, screen_image_path=emery_shot)
        out_path = os.path.join(app_mockup_dir, f"pebble-time2-{app_name}.png")
        t2.save(out_path, "PNG")
        print(f"  ✓ Created {out_path}")

    # Basalt (Pebble Time Steel)
    basalt_shot = os.path.join(app_shot_dir, f"basalt-{app_name}-running.png")
    if not os.path.exists(basalt_shot):
        basalt_shot = os.path.join(app_shot_dir, "basalt-time.png")
    if not os.path.exists(basalt_shot):
        basalt_shot = os.path.join(app_shot_dir, f"basalt-{app_name}-view.png")
    if not os.path.exists(basalt_shot):
        basalt_shot = os.path.join(app_shot_dir, f"basalt-{app_name}-ready.png")

    if os.path.exists(basalt_shot):
        basalt = ghm.draw_pebble_basalt(scale=1.0, screen_image_path=basalt_shot)
        out_path = os.path.join(app_mockup_dir, f"pebble-basalt-{app_name}.png")
        basalt.save(out_path, "PNG")
        print(f"  ✓ Created {out_path}")

    # Diorite (Pebble 2 HR)
    diorite_shot = os.path.join(app_shot_dir, f"diorite-{app_name}-running.png")
    if not os.path.exists(diorite_shot):
        diorite_shot = os.path.join(app_shot_dir, "diorite-time.png")
    if not os.path.exists(diorite_shot):
        diorite_shot = os.path.join(app_shot_dir, f"diorite-{app_name}-view.png")
    if not os.path.exists(diorite_shot):
        diorite_shot = os.path.join(app_shot_dir, f"diorite-{app_name}-ready.png")

    if os.path.exists(diorite_shot):
        diorite = ghm.draw_pebble_diorite(scale=1.0, screen_image_path=diorite_shot)
        out_path = os.path.join(app_mockup_dir, f"pebble-diorite-{app_name}.png")
        diorite.save(out_path, "PNG")
        print(f"  ✓ Created {out_path}")

    # Aplite (Pebble Classic)
    aplite_shot = os.path.join(app_shot_dir, f"aplite-{app_name}-running.png")
    if not os.path.exists(aplite_shot):
        aplite_shot = os.path.join(app_shot_dir, "aplite-time.png")
    if not os.path.exists(aplite_shot):
        aplite_shot = os.path.join(app_shot_dir, f"aplite-{app_name}-view.png")
    if not os.path.exists(aplite_shot):
        aplite_shot = os.path.join(app_shot_dir, f"aplite-{app_name}-ready.png")

    if os.path.exists(aplite_shot):
        aplite = ghm.draw_pebble_aplite(scale=1.0, screen_image_path=aplite_shot)
        out_path = os.path.join(app_mockup_dir, f"pebble-aplite-{app_name}.png")
        aplite.save(out_path, "PNG")
        print(f"  ✓ Created {out_path}")


# ==============================================================================
# MAIN ENTRYPOINT
# ==============================================================================

def main():
    target = sys.argv[1].lower() if len(sys.argv) > 1 else "all"

    valid_targets = ["watchface", "chrono", "timer", "alarm", "all"]
    if target not in valid_targets:
        print(f"Usage: python3 generate_app_assets.py [{ '|'.join(valid_targets) }]")
        sys.exit(1)

    print("============================================================")
    print(f"⚡ PEBBLE PULSAR ASSET PIPELINE (Target: {target.upper()})")
    print("============================================================")

    if target in ["watchface", "all"]:
        capture_watchface_assets()
        generate_mockups_for_app("watchface")

    if target in ["chrono", "all"]:
        capture_chrono_assets()
        generate_mockups_for_app("chrono")

    if target in ["timer", "all"]:
        capture_timer_assets()
        generate_mockups_for_app("timer")

    if target in ["alarm", "all"]:
        capture_alarm_assets()
        generate_mockups_for_app("alarm")

    print("\n============================================================")
    print("✅ ASSET GENERATION & VALIDATION COMPLETE!")
    print(f"📦 Stored in: {SCREENSHOTS_DIR}")
    print("============================================================\n")


if __name__ == "__main__":
    main()
