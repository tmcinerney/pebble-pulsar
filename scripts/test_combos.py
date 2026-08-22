#!/usr/bin/env python3
import subprocess
import time
import os

def run_pebble(cmd):
    full_cmd = f"devenv shell -- pebble {cmd}"
    res = subprocess.run(full_cmd, shell=True, capture_output=True, text=True, cwd="/Users/tmcinerney/Code/Public/pebble-pulsar")
    return res.stdout

def main():
    print("Building and installing...")
    run_pebble("build")
    run_pebble("install --emulator emery")
    time.sleep(1)

    print("1. Capturing Default (Ruby Red, Italic Slant, Steps Beads)...")
    run_pebble("screenshot --emulator emery /tmp/combo_default_italic.png")

    # Let's test with different configurations directly via C persistent settings or taking screenshots
    print("Screenshots captured!")

if __name__ == "__main__":
    main()
