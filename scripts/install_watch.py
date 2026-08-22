#!/usr/bin/env python3
import sys
import time
import uuid
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import WebsocketTransport
from libpebble2.services.install import AppInstaller
from libpebble2.protocol.apps import AppRunState, AppRunStateStart

def install(phone_ip="192.168.10.203", pbw_path="build/pebble-pulsar.pbw"):
    url = f"ws://{phone_ip}:9000/"
    print(f"Connecting to watch via phone at {url} ...")
    print("Ensure the Pebble app is OPEN on your phone with 'Developer Connection' enabled.")
    
    pebble = None
    max_retries = 15
    for attempt in range(1, max_retries + 1):
        try:
            transport = WebsocketTransport(url)
            pebble = PebbleConnection(transport)
            pebble.connect()
            pebble.run_async()
            break
        except Exception as e:
            if attempt < max_retries:
                print(f"  [Attempt {attempt}/{max_retries}] Waiting for Pebble App Developer Connection on {phone_ip}:9000 ...")
                time.sleep(2)
            else:
                print(f"\n❌ Could not connect to {url}.")
                print("Troubleshooting steps:")
                print("1. Open the Pebble app on your phone.")
                print("2. Go to Settings -> Developer -> Toggle 'Developer Connection' ON.")
                print(f"3. Confirm your phone's Wi-Fi IP is {phone_ip}.")
                sys.exit(1)
    
    print(f"\n✓ Connected to watch: {pebble.watch_info.board} (Platform {pebble.watch_info.running.hardware_platform}, FW {pebble.watch_info.running.version_tag})")
    
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

def uninstall(phone_ip="192.168.10.203", app_uuid_str="8b5f3a12-9c4e-4f71-8b23-6e4d7a8c9b01"):
    from libpebble2.services.blobdb import BlobDBClient, BlobDatabaseID, SyncWrapper
    url = f"ws://{phone_ip}:9000/"
    print(f"Connecting to watch via phone at {url} ...")
    transport = WebsocketTransport(url)
    pebble = PebbleConnection(transport)
    pebble.connect()
    pebble.run_async()
    print(f"✓ Connected to watch: {pebble.watch_info.board}")
    
    blobdb = BlobDBClient(pebble)
    target_uuid = uuid.UUID(app_uuid_str)
    print(f"Deleting app UUID {target_uuid} from watch BlobDB...")
    res = SyncWrapper(blobdb.delete, BlobDatabaseID.App, target_uuid).wait()
    print(f"✓ Delete status result: {res} (Success)")

if __name__ == "__main__":
    if "--uninstall" in sys.argv:
        ip = [arg for arg in sys.argv[1:] if arg != "--uninstall"]
        target_ip = ip[0] if ip else "192.168.10.203"
        uninstall(target_ip)
    else:
        ip = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith("-") else "192.168.10.203"
        pbw = sys.argv[2] if len(sys.argv) > 2 else "build/pebble-pulsar.pbw"
        install(ip, pbw)
