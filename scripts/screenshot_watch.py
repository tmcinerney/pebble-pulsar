import sys
from libpebble2.communication import PebbleConnection
from libpebble2.communication.transports.websocket import MessageTargetPhone, MessageTargetWatch, WebsocketTransport
from libpebble2.services.screenshot import Screenshot

transport = WebsocketTransport("ws://192.168.10.203:9000/")
pebble = PebbleConnection(transport)
pebble.connect()
pebble.run_async()

from PIL import Image

screenshot_service = Screenshot(pebble)
print("Capturing live screenshot from physical watch...")
rows = screenshot_service.grab_image()
height = len(rows)
width = len(rows[0]) // 3
raw_data = b"".join(rows)
image = Image.frombytes("RGB", (width, height), raw_data)
image.save("/tmp/watch_live_settings.png")
print("Saved screenshot to /tmp/watch_live_settings.png")

pebble.close()
