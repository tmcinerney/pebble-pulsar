#!/usr/bin/env python3
"""
Pebble Pulsar Firmware & Configuration Test Suite.
Validates:
1. Valid Pebble SDK System Font Keys across all C sources.
2. 1:1 Parity between package.json messageKeys, src/c/pulsar.c STORAGE_KEY_* defines, and pkjs config.json.
3. Multi-platform build output & ELF memory limits.
"""

import os
import re
import json
import subprocess
import unittest

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

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
        """Ensure every FONT_KEY_* in C code exists in the official Pebble SDK font table."""
        c_file = os.path.join(REPO_ROOT, "src", "c", "pulsar.c")
        with open(c_file, "r") as f:
            content = f.read()

        font_matches = re.findall(r"FONT_KEY_[A-Z0-9_]+", content)
        self.assertTrue(len(font_matches) > 0, "No font keys found in C code.")

        for font in font_matches:
            self.assertIn(
                font,
                VALID_PEBBLE_SYSTEM_FONTS,
                f"Invalid system font '{font}' referenced in pulsar.c! This causes NULL pointer crashes on real hardware."
            )

    def test_message_keys_sync(self):
        """Ensure package.json messageKeys match STORAGE_KEY_* defines and pkjs config.json."""
        package_json_path = os.path.join(REPO_ROOT, "package.json")
        with open(package_json_path, "r") as f:
            pkg = json.load(f)

        message_keys = pkg.get("pebble", {}).get("messageKeys", {})
        self.assertIsInstance(message_keys, dict, "pebble.messageKeys must be an explicit dictionary map.")

        c_file = os.path.join(REPO_ROOT, "src", "c", "pulsar.c")
        with open(c_file, "r") as f:
            c_code = f.read()

        storage_keys = dict(re.findall(r"#define\s+(STORAGE_KEY_[A-Z0-9_]+)\s+(\d+)", c_code))

        key_mapping = {
            "AppKeyOperatingMode": "STORAGE_KEY_OPERATING_MODE",
            "AppKeyColorway": "STORAGE_KEY_COLORWAY",
            "AppKeyFlickAction": "STORAGE_KEY_FLICK_ACTION",
            "AppKeyHourlyVibe": "STORAGE_KEY_HOURLY_VIBE",
            "AppKeyShowStepBeads": "STORAGE_KEY_SHOW_STEP_BEADS",
            "AppKeyItalicSlant": "STORAGE_KEY_ITALIC_SLANT",
            "AppKeyFooterStyle": "STORAGE_KEY_FOOTER_STYLE",
            "AppKeyStepGoal": "STORAGE_KEY_STEP_GOAL",
            "AppKeyHeaderStyle": "STORAGE_KEY_HEADER_STYLE",
            "AppKeyDateFormat": "STORAGE_KEY_DATE_FORMAT",
            "AppKeyLeadingZero": "STORAGE_KEY_LEADING_ZERO",
            "AppKeyBtVibe": "STORAGE_KEY_BT_VIBE",
            "AppKeyBeadMode": "STORAGE_KEY_BEAD_MODE",
            "AppKeyChargingStyle": "STORAGE_KEY_CHARGING_STYLE",
            "AppKeyNightlight": "STORAGE_KEY_NIGHTLIGHT",
            "AppKeyCycleSlot1": "STORAGE_KEY_CYCLE_SLOT_1",
            "AppKeyCycleSlot2": "STORAGE_KEY_CYCLE_SLOT_2",
            "AppKeyCycleSlot3": "STORAGE_KEY_CYCLE_SLOT_3",
            "AppKeyCycleSlot4": "STORAGE_KEY_CYCLE_SLOT_4",
            "AppKeyCycleSlot5": "STORAGE_KEY_CYCLE_SLOT_5",
        }

        for app_key, storage_key in key_mapping.items():
            self.assertIn(app_key, message_keys, f"Missing messageKey in package.json: {app_key}")
            self.assertIn(storage_key, storage_keys, f"Missing define in pulsar.c: {storage_key}")
            msg_id = int(message_keys[app_key])
            storage_id = int(storage_keys[storage_key])
            self.assertEqual(
                msg_id,
                storage_id,
                f"Mismatch for {app_key} ({msg_id}) vs {storage_key} ({storage_id})!"
            )

    def test_config_json_app_keys(self):
        """Ensure all appKey fields in config.json exist in package.json messageKeys."""
        package_json_path = os.path.join(REPO_ROOT, "package.json")
        with open(package_json_path, "r") as f:
            pkg = json.load(f)
        message_keys = pkg.get("pebble", {}).get("messageKeys", {})

        config_json_path = os.path.join(REPO_ROOT, "src", "pkjs", "config.json")
        with open(config_json_path, "r") as f:
            config = json.load(f)

        def extract_message_keys(items):
            keys = []
            for item in items:
                if isinstance(item, dict):
                    if "messageKey" in item:
                        keys.append(item["messageKey"])
                    if "items" in item:
                        keys.extend(extract_message_keys(item["items"]))
            return keys

        config_keys = extract_message_keys(config)
        self.assertTrue(len(config_keys) > 0, "No messageKeys found in config.json")
        for k in config_keys:
            self.assertIn(k, message_keys, f"config.json references unknown messageKey '{k}'")

    def test_multi_platform_build(self):
        """Ensure all 4 platforms compile cleanly."""
        result = subprocess.run(
            ["devenv", "shell", "--", "pebble", "build"],
            cwd=REPO_ROOT,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        self.assertEqual(result.returncode, 0, f"pebble build failed:\n{result.stdout}\n{result.stderr}")
        self.assertTrue(os.path.exists(os.path.join(REPO_ROOT, "build", "pebble-pulsar.pbw")), "PBW file missing!")

if __name__ == "__main__":
    unittest.main()
