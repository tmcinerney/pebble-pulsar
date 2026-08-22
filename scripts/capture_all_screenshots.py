#!/usr/bin/env python3
import subprocess
import time
import os

PLATFORMS = ["emery", "basalt", "diorite", "aplite"]
MODES = ["time", "seconds", "date", "steps", "battery"]

def run_cmd(cmd, check=True):
    print(f"Running: {cmd}")
    res = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if check and res.returncode != 0:
        print(f"Error ({res.returncode}): {res.stderr}")
    return res

def main():
    os.makedirs("screenshots", exist_ok=True)
    
    # 1. Build project
    print("--- Building pbw ---")
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
            print(f"Failed install on {platform}: {install_res.stderr}")
            time.sleep(2)
            run_cmd(f"devenv shell -- pebble install --emulator {platform}", check=False)
            
        time.sleep(2)
        
        for mode in MODES:
            out_file = f"screenshots/{platform}_{mode}.png"
            print(f"Capturing {out_file}...")
            run_cmd(f"devenv shell -- pebble screenshot --emulator {platform} {out_file}")
            time.sleep(0.5)
            # Advance to next mode via tap
            run_cmd(f"devenv shell -- pebble emu-tap --emulator {platform} --direction x+", check=False)
            time.sleep(0.5)
            
        run_cmd("devenv shell -- pebble kill", check=False)
        time.sleep(1)

    print("\nAll platform screenshots captured successfully!")

if __name__ == "__main__":
    main()
