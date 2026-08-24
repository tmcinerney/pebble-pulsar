#!/usr/bin/env python3
"""
install_watch.py

Installs Pulsar Suite apps directly to a physical Pebble watch over WiFi
via the Pebble mobile app Developer Connection bridge.

Usage:
  devenv shell -- python3 scripts/install_watch.py <watchface|chrono|timer|alarm|all> [PHONE_IP]
"""

import sys
import time
import uuid
import json
import os
import zipfile
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport, MessageTargetPhone
from libpebble2.communication.transports.websocket.protocol import WebSocketInstallBundle, WebSocketInstallStatus
from libpebble2.protocol.apps import AppRunState, AppRunStateStart

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEFAULT_PHONE_IP = "192.168.10.203"

APP_MAP = {
    "watchface": {
        "name": "Pulsar 1970 Watchface",
        "pbw": os.path.join(REPO_ROOT, "dist", "pebble-pulsar-watchface.pbw"),
        "pkg": os.path.join(REPO_ROOT, "apps", "watchface", "package.json")
    },
    "chrono": {
        "name": "Pulsar Chronograph",
        "pbw": os.path.join(REPO_ROOT, "dist", "pebble-pulsar-chrono.pbw"),
        "pkg": os.path.join(REPO_ROOT, "apps", "chrono", "package.json")
    },
    "timer": {
        "name": "Pulsar Countdown Timer",
        "pbw": os.path.join(REPO_ROOT, "dist", "pebble-pulsar-timer.pbw"),
        "pkg": os.path.join(REPO_ROOT, "apps", "timer", "package.json")
    },
    "alarm": {
        "name": "Pulsar Alarm Clock",
        "pbw": os.path.join(REPO_ROOT, "dist", "pebble-pulsar-alarm.pbw"),
        "pkg": os.path.join(REPO_ROOT, "apps", "alarm", "package.json")
    }
}

def get_app_uuid(app_key):
    pkg_path = APP_MAP[app_key]["pkg"]
    with open(pkg_path, "r") as f:
        data = json.load(f)
    return uuid.UUID(data["pebble"]["uuid"])

def connect_pebble(phone_ip):
    url = f"ws://{phone_ip}:9000/"
    print(f"\nConnecting to watch via phone at {url} ...")
    print("Ensure the Pebble app is OPEN on your phone with 'Developer Connection' enabled.")
    
    max_retries = 45
    for attempt in range(1, max_retries + 1):
        try:
            transport = WebsocketTransport(url)
            pebble = PebbleConnection(transport)
            pebble.connect()
            pebble.run_async()
            print(f"✓ Connected to phone WebSocket on {phone_ip}:9000", flush=True)
            return pebble, transport
        except Exception:
            if attempt < max_retries:
                print(f"  [{attempt}/{max_retries}] Waiting for Pebble app on {phone_ip}:9000 (toggle Developer Connection ON in app)...", flush=True)
                time.sleep(2)
            else:
                print(f"\n❌ Connection timed out to {url}.", flush=True)
                print("1. Open the Pebble app on your phone.")
                print("2. Go to Settings -> Developer Options -> Turn 'Developer Connection' ON.")
                print("3. Keep the Pebble app active on your screen.")
                return None, None

def install_app(pebble, transport, app_key):
    info = APP_MAP[app_key]
    pbw_path = info["pbw"]
    app_uuid = get_app_uuid(app_key)

    if not os.path.exists(pbw_path):
        print(f"❌ PBW bundle not found: {pbw_path}. Run 'npm run build' first.")
        return False

    with open(pbw_path, "rb") as f:
        bundle_data = f.read()

    print(f"\n▶ Installing {info['name']} ({len(bundle_data)} bytes)...", flush=True)
    transport.send_packet(WebSocketInstallBundle(pbw=bundle_data), target=MessageTargetPhone())

    print("  Waiting for install confirmation...", flush=True)
    try:
        result = pebble.read_transport_message(MessageTargetPhone, WebSocketInstallStatus, timeout=30)
        if result.status == WebSocketInstallStatus.StatusCode.Success:
            print(f"  ✓ {info['name']} installed successfully!", flush=True)
        else:
            print(f"  ⚠️ Install status: {result.status}", flush=True)
    except Exception as e:
        print(f"  Note: {e}", flush=True)

    # Launch the newly installed app
    try:
        pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid)))
        print(f"  ✓ Launched {info['name']} on watch!", flush=True)
    except Exception as e:
        print(f"  Could not send launch signal: {e}", flush=True)

    return True

def main():
    target = "watchface"
    phone_ip = DEFAULT_PHONE_IP

    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    for a in args:
        if a in APP_MAP or a == "all":
            target = a
        elif "." in a:
            phone_ip = a

    print("=" * 60)
    print(f"⚡ PEBBLE PULSAR SUITE — DIRECT HARDWARE INSTALLER")
    print(f"   Target: {target}")
    print(f"   Phone IP: {phone_ip}")
    print("=" * 60)

    pebble, transport = connect_pebble(phone_ip)
    if not pebble:
        sys.exit(1)

    try:
        pebble.fetch_watch_info()
        print(f"✓ Watch Hardware: {pebble.watch_info.board} (FW {pebble.watch_info.running.version_tag})", flush=True)
    except Exception:
        pass

    apps_to_install = list(APP_MAP.keys()) if target == "all" else [target]
    for app in apps_to_install:
        install_app(pebble, transport, app)
        if len(apps_to_install) > 1:
            time.sleep(3)

    print("\n✅ Installation complete! App is ready on your watch.")

if __name__ == "__main__":
    main()
