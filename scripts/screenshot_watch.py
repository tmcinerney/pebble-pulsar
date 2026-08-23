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
from PIL import Image
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.services.screenshot import Screenshot

DEFAULT_PHONE_IP = "192.168.10.203"
DEFAULT_OUTPUT = "screenshot.png"

def main():
    output_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_OUTPUT
    phone_ip = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_PHONE_IP

    url = f"ws://{phone_ip}:9000/"
    print(f"Connecting to Pebble at {url} ...")

    transport = WebsocketTransport(url)
    pebble = PebbleConnection(transport)
    pebble.connect()
    pebble.run_async()

    print("Connected! Requesting screenshot from watch...")
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
