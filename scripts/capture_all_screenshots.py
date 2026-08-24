#!/usr/bin/env python3
"""
capture_all_screenshots.py

Automates capturing pixel-perfect screenshots from the Pebble emulator for all
supported platforms (Emery, Basalt, Diorite, Aplite) across all 4 applications
in the Pebble Pulsar Suite:
1. Watchface (Pulsar 1970)
2. Chrono (Pulsar Chronograph)
3. Timer (Pulsar Countdown)
4. Alarm (Pulsar Alarm)
"""

import sys
import subprocess
import time
import os

PLATFORMS = ["emery", "basalt", "diorite", "aplite"]
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def run_cmd(cmd, cwd=REPO_ROOT, check=True):
    print(f"Running: {cmd} (in {cwd})")
    res = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    if check and res.returncode != 0:
        print(f"Error ({res.returncode}): {res.stderr}")
    return res

def capture_watchface():
    app_dir = os.path.join(REPO_ROOT, "apps", "watchface")
    out_dir = os.path.join(REPO_ROOT, "screenshots", "watchface")
    legacy_out_dir = os.path.join(REPO_ROOT, "screenshots")
    os.makedirs(out_dir, exist_ok=True)
    
    print("\n" + "=" * 50)
    print("Capturing Watchface Screenshots...")
    print("=" * 50)

    for platform in PLATFORMS:
        run_cmd("pebble kill", check=False)
        time.sleep(1)
        run_cmd(f"pebble install --emulator {platform}", cwd=app_dir, check=False)
        time.sleep(2)
        
        # Inject standard steps & battery
        run_cmd(f"pebble emu-steps --emulator {platform} 8420", check=False)
        run_cmd(f"pebble emu-battery --emulator {platform} --percent 85", check=False)
        if platform in ["emery", "diorite"]:
            run_cmd(f"pebble emu-heart-rate --emulator {platform} 72", check=False)
            
        # 1. Time Mode
        shot_path = os.path.join(out_dir, f"{platform}-time.png")
        run_cmd(f"pebble screenshot --emulator {platform} {shot_path}")
        run_cmd(f"pebble screenshot --emulator {platform} {os.path.join(legacy_out_dir, f'{platform}-time.png')}")
        time.sleep(0.5)

        # 2. Tap to cycle mode
        run_cmd(f"pebble emu-tap --emulator {platform} --direction x+", check=False)
        time.sleep(0.6)
        shot_path = os.path.join(out_dir, f"{platform}-seconds.png")
        run_cmd(f"pebble screenshot --emulator {platform} {shot_path}")
        run_cmd(f"pebble screenshot --emulator {platform} {os.path.join(legacy_out_dir, f'{platform}-seconds.png')}")

        run_cmd("pebble kill", check=False)
        time.sleep(1)

def capture_chrono():
    app_dir = os.path.join(REPO_ROOT, "apps", "chrono")
    out_dir = os.path.join(REPO_ROOT, "screenshots", "chrono")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 50)
    print("Capturing Chronograph Screenshots...")
    print("=" * 50)

    for platform in PLATFORMS:
        run_cmd("pebble kill", check=False)
        time.sleep(1)
        run_cmd(f"pebble install --emulator {platform}", cwd=app_dir, check=False)
        time.sleep(2)

        # Start stopwatch
        run_cmd(f"pebble emu-button --emulator {platform} select", check=False)
        time.sleep(1.5)
        # Lap
        run_cmd(f"pebble emu-button --emulator {platform} up", check=False)
        time.sleep(1.0)

        shot_path = os.path.join(out_dir, f"{platform}-chrono-running.png")
        run_cmd(f"pebble screenshot --emulator {platform} {shot_path}")

        run_cmd("pebble kill", check=False)
        time.sleep(1)

def capture_timer():
    app_dir = os.path.join(REPO_ROOT, "apps", "timer")
    out_dir = os.path.join(REPO_ROOT, "screenshots", "timer")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 50)
    print("Capturing Timer Screenshots...")
    print("=" * 50)

    for platform in PLATFORMS:
        run_cmd("pebble kill", check=False)
        time.sleep(1)
        run_cmd(f"pebble install --emulator {platform}", cwd=app_dir, check=False)
        time.sleep(2)

        # Start countdown
        run_cmd(f"pebble emu-button --emulator {platform} select", check=False)
        time.sleep(1.0)

        shot_path = os.path.join(out_dir, f"{platform}-timer-running.png")
        run_cmd(f"pebble screenshot --emulator {platform} {shot_path}")

        run_cmd("pebble kill", check=False)
        time.sleep(1)

def capture_alarm():
    app_dir = os.path.join(REPO_ROOT, "apps", "alarm")
    out_dir = os.path.join(REPO_ROOT, "screenshots", "alarm")
    os.makedirs(out_dir, exist_ok=True)

    print("\n" + "=" * 50)
    print("Capturing Alarm Screenshots...")
    print("=" * 50)

    for platform in PLATFORMS:
        run_cmd("pebble kill", check=False)
        time.sleep(1)
        run_cmd(f"pebble install --emulator {platform}", cwd=app_dir, check=False)
        time.sleep(2)

        shot_path = os.path.join(out_dir, f"{platform}-alarm-view.png")
        run_cmd(f"pebble screenshot --emulator {platform} {shot_path}")

        run_cmd("pebble kill", check=False)
        time.sleep(1)

def main():
    target = sys.argv[1] if len(sys.argv) > 1 else "all"
    if target in ["watchface", "all"]:
        capture_watchface()
    if target in ["chrono", "all"]:
        capture_chrono()
    if target in ["timer", "all"]:
        capture_timer()
    if target in ["alarm", "all"]:
        capture_alarm()

if __name__ == "__main__":
    main()
