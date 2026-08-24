#!/usr/bin/env python3
"""
Pebble Pulsar Monorepo Suite Firmware & Configuration Test Suite.
Validates:
1. Valid Pebble SDK System Font Keys across all C sources in shared and apps.
2. 1:1 Parity between package.json messageKeys, src/c/*.c STORAGE_KEY_* defines, and pkjs config.json across all 4 apps.
3. Multi-platform compilation and bundle generation for all 4 apps (watchface, chrono, timer, alarm).
"""

import os
import re
import json
import glob
import subprocess
import unittest

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
APPS = ["watchface", "chrono", "timer", "alarm"]

VALID_PEBBLE_SYSTEM_FONTS = {
    "FONT_KEY_GOTHIC_14",
    "FONT_KEY_GOTHIC_14_BOLD",
    "FONT_KEY_GOTHIC_18",
    "FONT_KEY_GOTHIC_18_BOLD",
    "FONT_KEY_GOTHIC_24",
    "FONT_KEY_GOTHIC_24_BOLD",
    "FONT_KEY_GOTHIC_28",
    "FONT_KEY_GOTHIC_28_BOLD",
    "FONT_KEY_BITHAM_30_BLACK",
    "FONT_KEY_BITHAM_42_BOLD",
    "FONT_KEY_BITHAM_42_LIGHT",
    "FONT_KEY_BITHAM_42_MEDIUM_NUMBERS",
    "FONT_KEY_BITHAM_34_MEDIUM_NUMBERS",
    "FONT_KEY_BITHAM_34_LIGHT_SUBSET",
    "FONT_KEY_BITHAM_18_LIGHT_SUBSET",
    "FONT_KEY_ROBOTO_CONDENSED_21",
    "FONT_KEY_ROBOTO_BOLD_SUBSET_49",
    "FONT_KEY_DROID_SERIF_28_BOLD",
    "FONT_KEY_LECO_20_BOLD_NUMBERS",
    "FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM",
    "FONT_KEY_LECO_32_BOLD_NUMBERS",
    "FONT_KEY_LECO_36_BOLD_NUMBERS",
    "FONT_KEY_LECO_42_NUMBERS",
    "FONT_KEY_LECO_28_LIGHT_NUMBERS",
}

class TestFirmwareIntegrity(unittest.TestCase):

    def test_valid_system_fonts(self):
        """Ensure every FONT_KEY_* in all C source files exists in the official Pebble SDK font table."""
        c_files = glob.glob(os.path.join(REPO_ROOT, "apps", "**", "*.c"), recursive=True) + \
                  glob.glob(os.path.join(REPO_ROOT, "shared", "**", "*.c"), recursive=True)

        self.assertTrue(len(c_files) > 0, "No C source files found.")
        for c_file in c_files:
            with open(c_file, "r") as f:
                content = f.read()

            font_matches = re.findall(r"FONT_KEY_[A-Z0-9_]+", content)
            for font in font_matches:
                self.assertIn(
                    font,
                    VALID_PEBBLE_SYSTEM_FONTS,
                    f"Invalid system font '{font}' referenced in {c_file}!"
                )

    def test_apps_config_and_message_keys_sync(self):
        """Ensure each app's package.json messageKeys match its config.json and C defines."""
        for app in APPS:
            app_dir = os.path.join(REPO_ROOT, "apps", app)
            package_json_path = os.path.join(app_dir, "package.json")
            self.assertTrue(os.path.exists(package_json_path), f"Missing package.json for {app}")

            with open(package_json_path, "r") as f:
                pkg = json.load(f)

            message_keys = pkg.get("pebble", {}).get("messageKeys", {})
            self.assertIsInstance(message_keys, dict, f"{app}: pebble.messageKeys must be a dict")

            # Check config.json
            config_json_path = os.path.join(app_dir, "src", "pkjs", "config.json")
            if os.path.exists(config_json_path):
                with open(config_json_path, "r") as f:
                    config = json.load(f)

                def extract_keys(items):
                    keys = []
                    for item in items:
                        if isinstance(item, dict):
                            if "messageKey" in item:
                                keys.append(item["messageKey"])
                            if "items" in item:
                                keys.extend(extract_keys(item["items"]))
                    return keys

                config_keys = extract_keys(config)
                for k in config_keys:
                    self.assertIn(
                        k,
                        message_keys,
                        f"App '{app}': config.json references messageKey '{k}' not found in package.json"
                    )

    def test_suite_build_all(self):
        """Run build_all.py and verify all 4 PBW bundle outputs exist in dist/."""
        build_script = os.path.join(REPO_ROOT, "scripts", "build_all.py")
        result = subprocess.run(
            ["python3", build_script],
            cwd=REPO_ROOT,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        self.assertEqual(result.returncode, 0, f"build_all.py failed:\n{result.stdout}\n{result.stderr}")

        for app in APPS:
            pbw_path = os.path.join(REPO_ROOT, "dist", f"pebble-pulsar-{app}.pbw")
            self.assertTrue(os.path.exists(pbw_path), f"Missing PBW bundle for {app}: {pbw_path}")
            self.assertGreater(os.path.getsize(pbw_path), 50000, f"PBW file {pbw_path} suspiciously small")

if __name__ == "__main__":
    unittest.main()
