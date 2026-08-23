#!/usr/bin/env python3
"""
build_all.py

Compiles all applications in the Pebble Pulsar Suite across all platforms:
- watchface (Pulsar 1970)
- chrono (Pulsar Chronograph)
- timer (Pulsar Countdown)
- alarm (Pulsar Alarm)

Outputs the resulting PBW bundles into `dist/`.
"""

import os
import sys
import subprocess
import shutil

APPS = ["watchface", "chrono", "timer", "alarm"]
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DIST_DIR = os.path.join(REPO_ROOT, "dist")

def main():
    os.makedirs(DIST_DIR, exist_ok=True)
    print("=" * 60)
    print("⚡ BUILDING PEBBLE PULSAR MONOREPO SUITE")
    print("=" * 60)

    failed = []
    for app in APPS:
        app_dir = os.path.join(REPO_ROOT, "apps", app)
        print(f"\n▶ Building app: {app} (in {app_dir})")
        res = subprocess.run(["pebble", "build"], cwd=app_dir)
        if res.returncode != 0:
            print(f"❌ Failed to build {app}")
            failed.append(app)
        else:
            # Copy pbw to dist
            build_dir = os.path.join(app_dir, "build")
            pbw_files = [f for f in os.listdir(build_dir) if f.endswith(".pbw")]
            for pbw in pbw_files:
                src_pbw = os.path.join(build_dir, pbw)
                dest_pbw = os.path.join(DIST_DIR, f"pebble-pulsar-{app}.pbw")
                shutil.copy2(src_pbw, dest_pbw)
                print(f"  ✓ Bundled: {dest_pbw} ({os.path.getsize(dest_pbw)} bytes)")

    print("\n" + "=" * 60)
    if failed:
        print(f"❌ SUITE BUILD FAILED: {', '.join(failed)}")
        sys.exit(1)
    else:
        print("✅ ALL SUITE APPLICATIONS BUILT SUCCESSFULLY!")
        print(f"📦 Dist artifacts available in: {DIST_DIR}")
        for f in os.listdir(DIST_DIR):
            if f.endswith(".pbw"):
                p = os.path.join(DIST_DIR, f)
                print(f"   - {f} ({os.path.getsize(p)} bytes)")
        print("=" * 60)

if __name__ == "__main__":
    main()
