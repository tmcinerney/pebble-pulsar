#!/usr/bin/env python3
import sys
import uuid
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.services.install import AppInstaller
from libpebble2.protocol.apps import AppRunState, AppRunStateStart

def install(phone_ip="192.168.10.203", pbw_path="build/pebble-pulsar.pbw"):
    print(f"Connecting to watch via phone at ws://{phone_ip}:9000/ ...")
    transport = WebsocketTransport(f"ws://{phone_ip}:9000/")
    pebble = PebbleConnection(transport)
    pebble.connect()
    pebble.run_async()
    
    print(f"Connected to watch: {pebble.watch_info.board} (Platform {pebble.watch_info.running.hardware_platform}, FW {pebble.watch_info.running.version_tag})")
    
    installer = AppInstaller(pebble, pbw_path)
    def progress(sent, total_sent, total_size):
        pct = int(total_sent * 100 / total_size) if total_size else 0
        print(f"\rInstalling: [{pct}%] {total_sent}/{total_size} bytes", end="")
    installer.register_handler("progress", progress)
    installer.install(force_install=True)
    print("\n✓ App installation complete!")
    
    # Launch watchface
    app_uuid = uuid.UUID("8b5f3a12-9c4e-4f71-8b23-6e4d7a8c9b01")
    pebble.send_packet(AppRunState(data=AppRunStateStart(uuid=app_uuid)))
    print("✓ Pulsar 1970 launched on watch!")

if __name__ == "__main__":
    ip = sys.argv[1] if len(sys.argv) > 1 else "192.168.10.203"
    pbw = sys.argv[2] if len(sys.argv) > 2 else "build/pebble-pulsar.pbw"
    install(ip, pbw)
