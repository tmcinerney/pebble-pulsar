#!/usr/bin/env python3
"""
screenshot_watch.py

Captures a screenshot directly from a physical Pebble watch over the WebSocket bridge
and saves it to a PNG image file.

Usage:
  devenv shell -- python3 scripts/screenshot_watch.py [OUTPUT_PATH] [PHONE_IP]
"""

import sys
import os
import time
import json
import uuid
from PIL import Image
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.services.screenshot import Screenshot
from libpebble2.protocol.apps import AppRunState, AppRunStateStart

DEFAULT_PHONE_IP = "192.168.10.203"
DEFAULT_OUTPUT = "screenshot.png"
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

APP_UUIDS = {
    "watchface": "76f1c4e1-229b-4e63-95c0-111111111101",
    "chrono": "76f1c4e1-229b-4e63-95c0-111111111102",
    "timer": "76f1c4e1-229b-4e63-95c0-111111111103",
    "alarm": "76f1c4e1-229b-4e63-95c0-111111111104"
}

def main():
    target_app = None
    output_path = DEFAULT_OUTPUT
    phone_ip = DEFAULT_PHONE_IP

    args = sys.argv[1:]
    while args:
        arg = args.pop(0)
        if arg in ("--app", "-a") and args:
            target_app = args.pop(0).lower()
        elif arg.endswith(".png"):
            output_path = arg
        elif "." in arg:
            phone_ip = arg
        elif arg in APP_UUIDS:
            target_app = arg

    url = f"ws://{phone_ip}:9000/"
    print(f"Connecting to Pebble at {url} ...")

    transport = WebsocketTransport(url)
    pebble = PebbleConnection(transport)
    pebble.connect()
    pebble.run_async()

    if target_app and target_app in APP_UUIDS:
        app_uuid = uuid.UUID(APP_UUIDS[target_app])
        print(f"Launching {target_app} ({app_uuid})...")
        pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid)))
        time.sleep(1.2)

    print("Requesting screenshot from watch...")
    shot_service = Screenshot(pebble)
    
    try:
        rows = shot_service.grab_image()
        height = len(rows)
        width = len(rows[0]) // 3
        print(f"Captured {width}x{height} image! Saving to {output_path}...")

        img = Image.new("RGB", (width, height))
        pixels = img.load()
        for y, row in enumerate(rows):
            for x in range(width):
                r = row[x * 3]
                g = row[x * 3 + 1]
                b = row[x * 3 + 2]
                pixels[x, y] = (r, g, b)

        img.save(output_path)
        print(f"✓ Screenshot successfully saved to: {output_path}")
    except Exception as e:
        print(f"❌ Failed to capture screenshot: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
