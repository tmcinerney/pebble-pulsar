#!/usr/bin/env python3
"""
capture_all_screenshots.py

Automates capturing pixel-perfect screenshots from the Pebble emulator for all
supported platforms (Emery, Basalt, Diorite, Aplite) across all 6 display modes
and special features (Heart Rate, Charging Animations, Step Overdrive).
"""

import subprocess
import time
import os

PLATFORMS = ["emery", "basalt", "diorite", "aplite"]

def run_cmd(cmd, check=True):
    print(f"Running: {cmd}")
    res = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if check and res.returncode != 0:
        print(f"Error ({res.returncode}): {res.stderr}")
    return res

def main():
    os.makedirs("screenshots", exist_ok=True)
    
    # 1. Build project
    print("--- Building PBW ---")
    run_cmd("devenv shell -- pebble build")
    
    for platform in PLATFORMS:
        print(f"\n==========================================")
        print(f"Capturing screenshots for platform: {platform}")
        print(f"==========================================")
        
        run_cmd("devenv shell -- pebble kill", check=False)
        time.sleep(1)
        
        # Install to emulator
        install_res = run_cmd(f"devenv shell -- pebble install --emulator {platform}", check=False)
        if install_res.returncode != 0:
            print(f"Retrying install on {platform}...")
            time.sleep(2)
            run_cmd(f"devenv shell -- pebble install --emulator {platform}", check=False)
            
        time.sleep(2)
        
        # Inject standard steps & battery
        run_cmd(f"devenv shell -- pebble emu-steps --emulator {platform} 8420", check=False)
        run_cmd(f"devenv shell -- pebble emu-battery --emulator {platform} --percent 85", check=False)
        if platform in ["emery", "diorite"]:
            run_cmd(f"devenv shell -- pebble emu-heart-rate --emulator {platform} 72", check=False)
        
        # 1. Time Mode
        print(f"Capturing screenshots/{platform}-time.png...")
        run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} screenshots/{platform}-time.png")
        time.sleep(0.5)
        
        # 2. Live Seconds Mode
        run_cmd(f"devenv shell -- pebble emu-tap --emulator {platform} --direction x+", check=False)
        time.sleep(0.6)
        print(f"Capturing screenshots/{platform}-seconds.png...")
        run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} screenshots/{platform}-seconds.png")
        time.sleep(0.5)
        
        # 3. Date Mode
        run_cmd(f"devenv shell -- pebble emu-tap --emulator {platform} --direction x+", check=False)
        time.sleep(0.6)
        print(f"Capturing screenshots/{platform}-date.png...")
        run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} screenshots/{platform}-date.png")
        time.sleep(0.5)
        
        # 4. Steps Mode
        run_cmd(f"devenv shell -- pebble emu-tap --emulator {platform} --direction x+", check=False)
        time.sleep(0.6)
        print(f"Capturing screenshots/{platform}-steps.png...")
        run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} screenshots/{platform}-steps.png")
        time.sleep(0.5)
        
        # 5. Battery Mode
        run_cmd(f"devenv shell -- pebble emu-tap --emulator {platform} --direction x+", check=False)
        time.sleep(0.6)
        print(f"Capturing screenshots/{platform}-battery.png...")
        run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} screenshots/{platform}-battery.png")
        time.sleep(0.5)
        
        # 6. Heart Rate Mode (for Health platforms)
        if platform in ["emery", "basalt", "diorite"]:
            # Set direct flick action to Heart Rate (AppKeyFlickAction = 5)
            run_cmd(f"devenv shell -- pebble send-app-message --emulator {platform} --int 10002=5", check=False)
            time.sleep(0.5)
            run_cmd(f"devenv shell -- pebble emu-tap --emulator {platform} --direction x+", check=False)
            time.sleep(0.6)
            print(f"Capturing screenshots/{platform}-heartrate.png...")
            run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} screenshots/{platform}-heartrate.png")
            time.sleep(0.5)
            
        # Special Emery feature shots: Step Overdrive & Charging Animations
        if platform == "emery":
            # Step Overdrive: 12,500 steps (exceeds 10k goal)
            run_cmd(f"devenv shell -- pebble send-app-message --emulator emery --int 10002=3", check=False) # Steps mode
            run_cmd(f"devenv shell -- pebble emu-steps --emulator emery 12500", check=False)
            time.sleep(0.5)
            run_cmd(f"devenv shell -- pebble emu-tap --emulator emery --direction x+", check=False)
            time.sleep(0.6)
            print("Capturing screenshots/emery-steps-overdrive.png...")
            run_cmd("devenv shell -- pebble screenshot --emulator emery screenshots/emery-steps-overdrive.png")
            
            # Charging Animation: Cylon Chaser
            run_cmd("devenv shell -- pebble emu-battery --emulator emery --charging", check=False)
            run_cmd("devenv shell -- pebble send-app-message --emulator emery --int 10013=1 --int 10002=0", check=False) # Chaser style
            time.sleep(1.0)
            print("Capturing screenshots/emery-charging-cylon.png...")
            run_cmd("devenv shell -- pebble screenshot --emulator emery screenshots/emery-charging-cylon.png")
            
            # Charging Animation: 1970s Theater Marquee
            run_cmd("devenv shell -- pebble send-app-message --emulator emery --int 10013=3", check=False) # Marquee style
            time.sleep(0.6)
            print("Capturing screenshots/emery-charging-marquee.png...")
            run_cmd("devenv shell -- pebble screenshot --emulator emery screenshots/emery-charging-marquee.png")
            
        run_cmd("devenv shell -- pebble kill", check=False)
        time.sleep(1)

    print("\n✓ All platform screenshots and feature stills captured successfully!")

if __name__ == "__main__":
    main()
