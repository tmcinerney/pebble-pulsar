#!/usr/bin/env python3
import sys
import time
import os
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport, MessageTargetPhone, MessageTargetWatch
from libpebble2.protocol.logs import AppLogMessage

PHONE_IP = sys.argv[1] if len(sys.argv) > 1 else "192.168.10.203"
URL = f"ws://{PHONE_IP}:9000/"

print(f"Connecting to Pebble logs at {URL} ...")
print("Make sure Developer Connection is enabled in the Pebble mobile app.\n")

try:
    transport = WebsocketTransport(URL)
    pebble = PebbleConnection(transport)
    pebble.connect()
    pebble.run_async()
    print("✓ Connected! Streaming watch logs (Ctrl+C to stop)...\n")
    
    def log_handler(packet):
        print(f"[{packet.filename}:{packet.line_number}] {packet.message}", flush=True)

    pebble.register_endpoint(AppLogMessage, log_handler)

    while True:
        time.sleep(0.5)
except KeyboardInterrupt:
    print("\nExiting log stream.")
except Exception as e:
    print(f"Connection error: {e}")
