#!/usr/bin/env python3
"""
Install a PBW to a Pebble watch via LAN developer connection.
Handles macOS Local Network sandbox routing by bridging 127.0.0.1 to the watch IP.
"""
import sys
import os
import socket
import subprocess
import threading
import time

def main():
    if len(sys.argv) < 2:
        pbw = "dist/pebble-pulsar-watchface.pbw"
    else:
        pbw = sys.argv[1]

    phone_ip = os.environ.get("PEBBLE_PHONE_IP", "192.168.10.149")
    if len(sys.argv) >= 3:
        phone_ip = sys.argv[2]

    # Verify target port 9000 is open via platform nc (avoids macOS TCC local network restrictions on Python)
    res = subprocess.run(["/usr/bin/nc", "-vz", "-G", "2", phone_ip, "9000"], capture_output=True)
    if res.returncode != 0:
        print(f"❌ Cannot connect to {phone_ip}:9000.")
        print("Please ensure the Pebble app is open on your phone with 'Dev Connection' toggled ON.")
        sys.exit(1)

    # Start local relay on 127.0.0.1:9000 -> /usr/bin/nc phone_ip 9000
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", 9000))
    srv.listen(5)

    stop_event = threading.Event()

    def bridge_loop():
        while not stop_event.is_set():
            try:
                srv.settimeout(1.0)
                conn, _ = srv.accept()
            except socket.timeout:
                continue
            except Exception:
                break

            p = subprocess.Popen(["/usr/bin/nc", phone_ip, "9000"],
                                 stdin=subprocess.PIPE, stdout=subprocess.PIPE)

            def c2p():
                try:
                    while not stop_event.is_set():
                        data = conn.recv(4096)
                        if not data: break
                        p.stdin.write(data)
                        p.stdin.flush()
                except Exception:
                    pass
                finally:
                    try: p.stdin.close()
                    except: pass

            def p2c():
                try:
                    while not stop_event.is_set():
                        data = p.stdout.read1(4096)
                        if not data: break
                        conn.sendall(data)
                except Exception:
                    pass
                finally:
                    try: conn.close()
                    except: pass

            t1 = threading.Thread(target=c2p, daemon=True)
            t2 = threading.Thread(target=p2c, daemon=True)
            t1.start()
            t2.start()

    relay_thread = threading.Thread(target=bridge_loop, daemon=True)
    relay_thread.start()
    time.sleep(0.3)

    print(f"📲 Installing {pbw} to {phone_ip}...")
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    full_pbw = os.path.join(repo_root, pbw) if not os.path.isabs(pbw) else pbw

    cmd = ["devenv", "shell", "--", "pebble", "install", "--phone", "127.0.0.1", full_pbw]
    res = subprocess.run(cmd, cwd=repo_root)

    stop_event.set()
    srv.close()
    sys.exit(res.returncode)

if __name__ == "__main__":
    main()
