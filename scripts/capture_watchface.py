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
# AIDEV-NOTE: Unix seconds, NOT "HH:MM:SS". emu-set-time accepts the HH:MM:SS form, exits 0, and
# silently leaves the clock alone -- captures then carry whatever the wall clock said.
CLOCK_EPOCH = 1788368970    # 2026-09-02 10:09:30 local
STEPS = 8420          # 84% of the default 10,000 goal: a partly filled bead row reads better than a full one
HEART_RATE = 72
BATTERY = 85

# Message keys, from apps/watchface/package.json.
K_COLORWAY, K_GHOST, K_GLOW, K_HEADER, K_FOOTER = 10001, 10025, 10028, 10008, 10006
K_SLOT1 = 10015

# (label, how many taps from the Time screen). The GIF shows the paging interaction, which is the
# thing stills cannot convey, so it covers every screen.
SCREENS = [("time", 0), ("seconds", 1), ("date", 2), ("steps", 3), ("battery", 4)]

# Stills sell the look rather than the interaction, so they show the Time screen in each colourway.
# Values match enum ColorwayId in shared/include/pulsar_palette.h.
COLOURWAYS = [(0, "ruby"), (3, "amber"), (2, "green"), (4, "cyan")]


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


def footer_signature(path):
    """Identify the screen by its footer text band.

    AIDEV-NOTE: The footer literally names the screen -- TIME COMPUTER, SECONDS, DATE, STEPS, BATTERY --
    so it is the one region that identifies where we are. A whole-frame histogram cannot: the colon
    blinks, so two captures of the Time screen differ and a duplicate reads as a new screen. That is
    exactly how an off-by-one crept in, saving Time as "seconds", seconds as "date", and so on.
    """
    from PIL import Image
    im = Image.open(path).convert("L")
    w, h = im.size
    band = im.crop((0, int(h * 0.82), w, int(h * 0.95)))
    return tuple(band.getdata())


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
    for label, cmd in [
        ("clock", ["pebble", "emu-set-time", "--emulator", platform, str(CLOCK_EPOCH)]),
        ("steps", ["pebble", "emu-steps", "--emulator", platform, str(STEPS)]),
    ]:
        r = run(cmd, check=False)
        if r.returncode != 0:
            print(f"   WARNING: could not set {label}: {r.stderr.strip()[:120]}")
    run(["pebble", "emu-battery", "--emulator", platform, "--percent", str(BATTERY)], check=False)
    run(["pebble", "emu-heart-rate", "--emulator", platform, str(HEART_RATE)], check=False)
    # Ship-default appearance: ruby, glow on, ghost dots off, header and footer present.
    run(["pebble", "send-app-message", "--emulator", platform, "--int",
         f"{K_COLORWAY}=0", f"{K_GHOST}=0", f"{K_GLOW}=1",
         f"{K_HEADER}=0", f"{K_FOOTER}=0"], check=False)
    time.sleep(1.5)


def capture_screens(platform, attempts=3):
    """Walk the cycle in one pass: tap, capture, tap, capture.

    AIDEV-NOTE: Do NOT wait for the wake timer between screens. It returns the display to Time after
    WAKE_DURATION_MS, and a `pebble` invocation costs a second or more, so any pause long enough to feel
    safe is long enough to lose the screen -- which is how an earlier version silently saved Time under
    four different names. Capturing straight after each tap keeps the whole walk inside one wake, and a
    frame matching the Time frame means we desynced, so the pass restarts.
    """
    for attempt in range(attempts):
        # Settle on Time first, so the walk starts from a known screen.
        time.sleep(6)
        saved, previous_sig, desync = [], None, False

        for idx, (name, _) in enumerate(SCREENS):
            if idx > 0:
                run(["pebble", "emu-tap", "--emulator", platform, "--direction", "x+"], check=False)
                # Long enough for the tap to be handled and the layer redrawn, short enough that the
                # capture still lands inside WAKE_DURATION_MS. Without it the screenshot catches the
                # previous screen and every frame shifts by one.
                time.sleep(0.7)

            path = os.path.join(OUT, f"{platform}-{name}.png")
            if not shot(platform, path):
                desync = True
                break

            ok, why = is_plausible(path)
            if not ok:
                print(f"      {name}: {why}")
                desync = True
                break

            sig = footer_signature(path)
            if previous_sig is not None and sig == previous_sig:
                print(f"      {name}: footer unchanged, pass desynced")
                desync = True
                break
            previous_sig = sig

            saved.append(path)
            print(f"   {name:9} -> {os.path.basename(path)}")

        if not desync and len(saved) == len(SCREENS):
            return saved
        print(f"   restarting pass ({attempt + 1}/{attempts})")

    return []


def capture_colourways(platform):
    """One Time-screen still per colourway."""
    saved = []
    for value, name in COLOURWAYS:
        run(["pebble", "send-app-message", "--emulator", platform, "--int",
             f"{K_COLORWAY}={value}"], check=False)
        time.sleep(1.5)
        path = os.path.join(OUT, f"{platform}-colour-{name}.png")
        for _ in range(4):
            if shot(platform, path):
                ok, why = is_plausible(path)
                if ok:
                    saved.append(path)
                    print(f"   {name:9} -> {os.path.basename(path)}")
                    break
                print(f"      retry ({why})")
            time.sleep(1.5)
        else:
            print(f"   {name:9} FAILED")
    # Leave the emulator on the shipping default.
    run(["pebble", "send-app-message", "--emulator", platform, "--int",
         f"{K_COLORWAY}=0"], check=False)
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
    ap.add_argument("--colours", action="store_true",
                    help="also capture one Time-screen still per colourway")
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
        # AIDEV-NOTE: Repeated emulator restarts corrupt qemu_spi_flash.bin, after which installs still
        # report success but every screenshot and ping times out. It presents as a hung emulator sitting
        # on the boot screen at 100% CPU. kill+wipe is the documented recovery, and starting each session
        # from a clean flash stops it recurring.
        run(["pebble", "kill"], check=False, timeout=60)
        run(["pebble", "wipe"], check=False, timeout=60)
        run(["pebble", "install", "--emulator", platform], check=False, timeout=420)
        if not wait_for_emulator(platform):
            print("   emulator never became ready")
            failed.append(platform)
            continue
        apply_state(platform)
        saved = capture_screens(platform)
        if len(saved) != len(SCREENS):
            failed.append(platform)
        if len(saved) >= 2:
            build_gif(saved, os.path.join(OUT, f"{platform}-cycle.gif"))
        if args.colours:
            capture_colourways(platform)

    print("\n" + ("FAILED: " + ", ".join(failed) if failed else "all platforms captured"))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
