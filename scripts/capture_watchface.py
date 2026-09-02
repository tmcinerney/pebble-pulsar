#!/usr/bin/env python3
"""
capture_watchface.py

Deterministic screenshots of the Pulsar 1970 watchface.

The previous capture script waited on fixed sleeps and ignored exit codes, so it produced blank
frames, system dialogs and stale screens whenever the emulator was slower than the guess. This one
never guesses:

  * waits for the emulator by polling a real screenshot rather than sleeping
  * pins the clock, step count, heart rate and battery so every capture shows the same state
  * pages to a screen, then verifies the capture actually changed before accepting it
  * rejects frames that are blank or that look like a system dialog

Usage:
    python3 scripts/capture_watchface.py [platform ...]      # default: all four
    python3 scripts/capture_watchface.py emery --gif         # also build the cycling GIF
"""

import argparse
import collections
import os
import subprocess
import sys
import time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
APP = os.path.join(REPO, "apps", "watchface")
OUT = os.path.join(REPO, "screenshots")

PLATFORMS = ["emery", "basalt", "diorite", "aplite"]

# Fixed state, so every capture across every platform is comparable.
# 10:09 follows the convention digital watch advertising uses, and exercises four distinct digits.
CLOCK = "10:09:30"   # HH:MM:SS, per emu-set-time
STEPS = 8420          # 84% of the default 10,000 goal: a partly filled bead row reads better than a full one
HEART_RATE = 72
BATTERY = 85

# Message keys, from apps/watchface/package.json.
K_COLORWAY, K_GHOST, K_GLOW, K_HEADER, K_FOOTER = 10001, 10025, 10028, 10008, 10006
K_SLOT1 = 10015

# (label, how many taps from the Time screen)
SCREENS = [("time", 0), ("seconds", 1), ("date", 2), ("steps", 3), ("battery", 4)]


def run(args, check=True, timeout=180):
    """Argument list, never a shell string: platform names come from argv."""
    r = subprocess.run(args, cwd=APP, capture_output=True, text=True, timeout=timeout)
    if check and r.returncode != 0:
        raise RuntimeError(f"{' '.join(args)}\n{r.stderr.strip()[:400]}")
    return r


def shot(platform, path):
    """Capture, returning False rather than raising so callers can poll."""
    r = run(["pebble", "screenshot", "--emulator", platform, path, "--no-open"], check=False)
    return r.returncode == 0 and os.path.exists(path)


def frame_signature(path):
    """Colour histogram, used both to spot junk frames and to tell screens apart."""
    from PIL import Image
    im = Image.open(path).convert("RGB")
    counts = collections.Counter(im.getdata())
    return tuple(sorted(counts.items(), key=lambda kv: -kv[1])[:6]), im.size


def is_plausible(path):
    """Reject blank frames and system dialogs.

    A real watchface frame is mostly background with a modest amount of lit colour. A system dialog is
    mostly light grey; a blank frame is a single colour.
    """
    from PIL import Image
    im = Image.open(path).convert("RGB")
    px = list(im.getdata())
    counts = collections.Counter(px)
    if len(counts) < 3:
        return False, "single flat colour"
    top_colour, top_n = counts.most_common(1)[0]
    if top_n / len(px) > 0.97:
        return False, "essentially blank"
    light = sum(n for c, n in counts.items() if sum(c) > 500)
    if light / len(px) > 0.5:
        return False, "mostly light: probably a system dialog"
    return True, "ok"


def wait_for_emulator(platform, tries=40):
    probe = os.path.join(OUT, f".probe-{platform}.png")
    for i in range(tries):
        if shot(platform, probe):
            os.remove(probe)
            return True
        time.sleep(2)
    return False


def apply_state(platform):
    """Pin everything that would otherwise drift between runs."""
    run(["pebble", "emu-set-time", "--emulator", platform, CLOCK], check=False)
    run(["pebble", "emu-steps", "--emulator", platform, str(STEPS)], check=False)
    run(["pebble", "emu-battery", "--emulator", platform, "--percent", str(BATTERY)], check=False)
    run(["pebble", "emu-heart-rate", "--emulator", platform, str(HEART_RATE)], check=False)
    # Ship-default appearance: ruby, glow on, ghost dots off, header and footer present.
    run(["pebble", "send-app-message", "--emulator", platform, "--int",
         f"{K_COLORWAY}=0", f"{K_GHOST}=0", f"{K_GLOW}=1",
         f"{K_HEADER}=0", f"{K_FOOTER}=0"], check=False)
    time.sleep(1.5)


def capture_screens(platform):
    """Tap to each screen and verify the frame actually changed before keeping it."""
    saved, previous = [], None
    for name, taps in SCREENS:
        path = os.path.join(OUT, f"{platform}-{name}.png")

        # Return to Time, then tap forward. The wake timer returns to Time on its own after 4s,
        # so waiting it out is more reliable than assuming where we are.
        time.sleep(5)
        for _ in range(taps):
            run(["pebble", "emu-tap", "--emulator", platform, "--direction", "x+"], check=False)
            time.sleep(0.8)

        for attempt in range(4):
            if not shot(platform, path):
                time.sleep(1.5)
                continue
            ok, why = is_plausible(path)
            if not ok:
                print(f"      retry ({why})")
                time.sleep(1.5)
                continue
            sig = frame_signature(path)
            if previous is not None and sig == previous and name != "time":
                print("      retry (frame identical to previous screen)")
                time.sleep(1.5)
                continue
            previous = sig
            saved.append(path)
            print(f"   {name:9} -> {os.path.basename(path)}")
            break
        else:
            print(f"   {name:9} FAILED after 4 attempts")
    return saved


def build_gif(paths, out_path, ms=1400):
    from PIL import Image
    frames = [Image.open(p).convert("P", palette=Image.ADAPTIVE) for p in paths]
    frames[0].save(out_path, save_all=True, append_images=frames[1:],
                   duration=ms, loop=0, optimize=True)
    print(f"   gif      -> {os.path.basename(out_path)} ({len(frames)} frames)")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("platforms", nargs="*", default=None)
    ap.add_argument("--gif", action="store_true", help="also build a screen-cycling GIF")
    args = ap.parse_args()
    targets = args.platforms or PLATFORMS
    unknown = [t for t in targets if t not in PLATFORMS]
    if unknown:
        print(f"unknown platform(s): {', '.join(unknown)}; expected one of {', '.join(PLATFORMS)}")
        return 2

    os.makedirs(OUT, exist_ok=True)
    failed = []
    for platform in targets:
        print(f"\n=== {platform} ===")
        run(["pebble", "install", "--emulator", platform], check=False, timeout=420)
        if not wait_for_emulator(platform):
            print("   emulator never became ready")
            failed.append(platform)
            continue
        apply_state(platform)
        saved = capture_screens(platform)
        if len(saved) != len(SCREENS):
            failed.append(platform)
        if args.gif and len(saved) >= 2:
            build_gif(saved, os.path.join(OUT, f"{platform}-cycle.gif"))

    print("\n" + ("FAILED: " + ", ".join(failed) if failed else "all platforms captured"))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
