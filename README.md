# Pulsar 1970 (Pebble Time 2 / Pebble Time Watchface)

<p align="center">
  <img src="screenshots/pulsar-time2-hero.png" width="100%" alt="Pulsar 1970 Hero Banner">
</p>

<p align="center">
  <img src="screenshots/renders/pebble-time2-3d-angle.png" width="48%" alt="3D Pebble Time 2 Studio Render" />
  <img src="screenshots/renders/pebble-pulsar-flick-actions-optimized.gif" width="48%" alt="Pulsar 1970 Wrist Flick Gesture Animation" />
</p>

<p align="center">
  <em>Left: 3D studio product render of the Pebble Time 2. Right: Real-time wrist-flick gesture cycling across all 5 display modes.</em>
</p>

A high-fidelity retro digital watchface inspired by the iconic **1970s Hamilton Pulsar** ("Time Computer" P1, P2, and P3 models)—the world's first commercial digital LED wristwatch.

Built for the **Rebble / RePebble** ecosystem with native support for the **Pebble Time 2 (`emery`)**, **Pebble Time / Time Steel (`basalt`)**, **Pebble 2 (`diorite`)**, and classic **Pebble (`aplite`)**.

---

## Features & Configurable Capabilities

### 1. Authentic 1970s LED Matrix & Visuals
- **5×7 GaAsP Dot-Matrix LEDs:** Mathematically rendered circular LED dies that scale crisply across all display resolutions.
- **Authentic ~6° Vintage Italic Slant:** Replicates the physical rightward die tilt characteristic of 1970s Litronix/Bowmar LED modules (toggleable in settings).
- **GaAsP Ghost Matrix:** Unlit LED dies are rendered in deep maroon (`GColorBulgarianRose`) on a pitch-black background (`GColorBlack`), mimicking unlit semiconductor dies beneath synthetic ruby mineral crystal.
- **Pure Seamless Black Background:** Borderless design that integrates seamlessly with the physical watch bezel and glass.
- **Noise-Free 1-Bit Monochrome Rendering:** Clean, pure black background on Pebble 2 (`diorite`) and Classic (`aplite`) with zero dither noise and solid high-contrast white LED dies.
- **Customizable Bezel Footer:** Choose between `TIME COMPUTER` (1972 Hamilton homage), `HAMILTON`, `PULSAR`, or `None (Clean / Minimal)`.

### 2. Multi-Mode Display Engine (Wrist Flick / Tap)
Cycle through 5 interactive display modes via wrist flick or tap:
1. **Time Mode (`HH:MM`):** 12h/24h time with blinking GaAsP colon and AM/PM LED indicator dot.
2. **Live Seconds Mode (`:SS`):** Real-time ticking seconds counter.
3. **Calendar Date Mode (`MM DD`):** Space-age month and day readout.
4. **Step Counter Mode (`08420`):** 5-digit daily step count powered by the Pebble Health API.
5. **Battery Level Mode (` 85 %` / `100 %`):** Digital battery percentage monitor.

### 3. LED Colorways
- **Ruby Red (GaAsP 1972):** Default deep red semiconductor LED with maroon ghost dies.
- **Prototype Green (GaP 1975):** Vintage green LED homage.
- **Amber Gold (HP-01 Style):** Warm golden-yellow space-age calculation watch homage.
- **Cobalt Blue:** Modern electric blue styling.

### 4. Health & Alerts
- **10-Dot Micro-LED Step Progress Bar:** 10 discrete micro-dots showing daily progress towards a configurable step goal (5,000 to 15,000 steps).
- **Stealth Push-to-Wake Mode:** Replicates the original Pulsar power-saving behavior—display remains dark until awakened for 4 seconds by wrist movement.
- **Hourly Vibration Chime:** Configurable silent hourly alert (Off, Single Pulse, Double Pulse).
- **Status Indicators:** Subtle LED status dots for Bluetooth disconnect and battery low (<20%).

---

## Multi-Platform Hardware Support

**Pulsar 1970** includes custom geometry, scaling, and color rendering for rectangular Pebble smartwatches:

![All Pebble Platforms Supported](screenshots/pulsar-all-platforms-mockup.png)

- **Pebble Time 2 (`emery`):** 200×228 64-color resolution with large 5×7 GaAsP dies and full breathing room.
- **Pebble Time / Time Steel (`basalt`):** 144×168 64-color screen with full vintage colorway options.
- **Pebble 2 HR / SE (`diorite`):** High-contrast 1-bit black & white display with noise-free white LED dies.
- **Pebble Classic / Steel (`aplite`):** 1-bit monochrome memory LCD support.

---

## Modes Preview (Pebble Time 2)

| Time (`17:46`) | Live Seconds (`:52`) | Date (`08.21`) | Daily Steps (`08420`) | Battery (`85%`) |
| :---: | :---: | :---: | :---: | :---: |
| ![Time](screenshots/emery-time.png) | ![Seconds](screenshots/emery-seconds.png) | ![Date](screenshots/emery-date.png) | ![Steps](screenshots/emery-steps.png) | ![Battery](screenshots/emery-battery.png) |

---

## App Store Assets

| Rebble Appstore Banner (720×320) | Store Icon (260×260) |
| :---: | :---: |
| ![Banner](screenshots/appstore-banner-720x320.png) | ![Icon](screenshots/store-icon-260x260.png) |

---

## Developer Workflow & Direct Installation

This repository uses [`devenv.nix`](devenv.nix) providing the official ARM GCC toolchain and Pebble SDK 4.33.1.

### 1. Build Watchface Bundle
```bash
devenv shell -- pebble build
```
This produces `build/pebble-pulsar.pbw` containing binaries and Clay configuration for all supported platforms (`emery`, `basalt`, `diorite`, `aplite`).

### 2. Test in Emulator
```bash
# Pebble Time 2 (200x228 Color)
devenv shell -- pebble install --emulator emery

# Simulate wrist flick / tap
devenv shell -- pebble emu-tap --emulator emery --direction x+
```

### 3. Direct Watch Installation (Standard WiFi Developer Bridge)
To flash the watch directly without manual web downloads:
1. Ensure the Pebble mobile app is open with **Developer Connection** enabled (Settings → Developer Connection).
2. Run the direct installer:
```bash
devenv shell -- python3 scripts/install_watch.py <PHONE_IP>
```
This uses the official `libpebble2` protocol engine to stream the app via `PutBytes` and register it in the watch's internal `BlobDB` database.

---

## Rebble App Store Distribution

To distribute **Pulsar 1970** to all Pebble users worldwide:

1. **Rebble Developer Portal:**
   - Log in at [dev-portal.rebble.io](https://dev-portal.rebble.io/) using your Rebble account.
2. **Create New Application:**
   - Select **Watchface**.
   - Upload `build/pebble-pulsar.pbw`.
   - Title: **Pulsar 1970**
   - Description: Highlight the 1970s Hamilton Pulsar heritage, 5 interactive modes, and customizable colorways.
   - Screenshots: Upload the provided screenshots from the `screenshots/` directory.
3. **Publish:**
   - Once submitted, the watchface is immediately indexable in the Rebble App Store and installable directly from the mobile Pebble app with working Clay settings.

---

## Historical Note: Why "TIME COMPUTER"?

When the Hamilton Watch Company unveiled the original Pulsar in 1970 on the *Tonight Show Starring Johnny Carson*, it had no moving parts, gears, or hands—an unprecedented leap in electronics. In 1972, Hamilton incorporated the dedicated subsidiary **"Pulsar Time Computer, Inc."** All original P1, P2 ("Astronaut"), and P3 timepieces were branded as *Pulsar Time Computers*.

---

## License

MIT © Travers McInerney
