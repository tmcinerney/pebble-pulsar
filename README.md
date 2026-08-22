# Pulsar 1970 (Pebble Time 2 / Pebble Time Watchface)

<p align="center">
  <img src="screenshots/pulsar-time2-hero.png" width="100%" alt="Pulsar 1970 Hero Banner">
</p>

<p align="center">
  <img src="screenshots/mockups/pebble-time2-device.png" width="44%" alt="Pebble Time 2 Hardware Mockup" />
  <img src="screenshots/renders/pebble-pulsar-flick-actions-optimized.gif" width="44%" alt="Pulsar 1970 Wrist Flick Gesture Animation" />
</p>

<p align="center">
  <em>Left: Authentic Pebble Time 2 hardware in 316L brushed stainless steel with procedural GaAsP LED matrix. Right: Live wrist-flick gesture animation cycling through all 5 display modes.</em>
</p>

A high-fidelity retro digital watchface inspired by the iconic **1970s Hamilton Pulsar** ("Time Computer" P1, P2, and P3 models)—the world's first commercial digital LED wristwatch.

Built for the **Rebble / RePebble** ecosystem with native support for the **Pebble Time 2 (`emery`)**, **Pebble Time / Time Steel (`basalt`)**, **Pebble 2 (`diorite`)**, and classic **Pebble (`aplite`)**.

---

## Quick Start & Interaction Guide

### 💡 Operating Modes
* **Always-On Mode (Default):** The dot-matrix time is continuously visible with blinking colon and active status dots.
* **Stealth Mode (Authentic 1972 Experience):** Replicates the real 1970s Pulsar P1/P2/P3 experience where the LED display remains completely dark to conserve power beneath the synthetic ruby crystal. 
  * **To read the time:** **Flick your wrist or gently tap the watch**. The time will brightly illuminate for 4 seconds before fading back to sleep.
  * **To view other stats:** Subsequent taps while awake cycle through Live Seconds (`:SS`), Date, Daily Steps, and Battery.

### 👋 Wrist Flick & Tap Actions
In the Pebble mobile app settings, you can configure what a wrist flick does:
* **Cycle All (Default):** Temporarily switches through Time &rarr; Live Seconds &rarr; Date &rarr; Steps &rarr; Battery, automatically returning to Time after 4 seconds.
* **Live Seconds Counter (`:SS`):** Instant 4-second burst of real-time ticking seconds.
* **Calendar Date (`MM DD` / `DD MM`):** Quick check of current month and day.
* **Step Count (`08420`):** Immediate readout of your daily Pebble Health steps.
* **Battery Level (` 85 %`):** Battery percentage readout.

---

## Features & Capabilities

### 1. Authentic 1970s LED Matrix & Visuals
- **5×7 GaAsP Dot-Matrix LEDs:** Mathematically rendered circular LED dies that scale crisply across all display resolutions.
- **Authentic 12° Vintage Italic Slant:** Replicates the physical rightward die tilt characteristic of 1970s Litronix/Bowmar LED modules (toggleable in Clay settings for upright block mode).
- **GaAsP Ghost Matrix:** Unlit LED dies are rendered in deep maroon (`GColorBulgarianRose`) on a pitch-black background (`GColorBlack`), mimicking unlit semiconductor dies beneath synthetic ruby mineral crystal.
- **Pure Seamless Black Background:** Borderless design that integrates seamlessly with the physical watch bezel and glass.
- **Noise-Free 1-Bit Monochrome Rendering:** Clean, pure black background on Pebble 2 (`diorite`) and Classic (`aplite`) with zero dither noise and solid high-contrast white LED dies.
- **Customizable Brand Headers & Footers:** Choose top header (`PULSAR`, `HAMILTON`, `SOLID STATE`, or `None`) and bezel footer (`TIME COMPUTER`, `SOLID STATE`, `HAMILTON`, `PULSAR`, `SWISS MADE`, or `None`).

### 2. Multi-Mode Display Engine (Wrist Flick / Tap)
Cycle through 5 interactive display modes via wrist flick or tap:
1. **Time Mode (`HH:MM`):** 12h/24h time with blinking GaAsP colon, optional leading zero, and AM/PM LED indicator dot.
2. **Live Seconds Mode (`:SS`):** Real-time ticking seconds counter.
3. **Calendar Date Mode (`MM DD` / `DD MM`):** Space-age month and day readout with configurable format.
4. **Step Counter Mode (`08420`):** 5-digit daily step count powered by the Pebble Health API.
5. **Battery Level Mode (` 85 %` / `100 %`):** Digital battery percentage monitor.

### 3. LED Colorways & Inverted Theme
- **Neon Ruby (GaAsP High-Luminance):** Vivid red semiconductor LED with maroon ghost dies.
- **Deep Red (Classic 1972):** Traditional deep ruby crystal homage.
- **Prototype Green (GaP 1975):** Vintage green LED homage.
- **Amber Gold (HP-01 Style):** Warm golden-yellow space-age calculation watch homage.
- **Cobalt Blue:** Modern electric blue styling.
- **Lunar White:** High-contrast crisp white LED dies.
- **Inverted Paper (Black on White):** Crisp black matrix LEDs on a paper-white background for high-visibility bright outdoor sunlight.

### 4. Health, Progress & Alerts
- **10-Dot Micro-LED Progress Bar:** Multi-mode indicator above the footer:
  - **Daily Step Progress:** 10 micro-dots showing progress towards your daily step goal (5k to 15k).
  - **Battery Meter:** 1 dot = 10% battery charge level.
  - **Disabled / Off:** Clean minimal bezel.
- **Stealth Push-to-Wake Mode:** 4-second flick/tap illumination.
- **Hourly Vibration Chime:** Configurable silent hourly alert (Off, Single Pulse, Double Pulse).
- **Bluetooth Disconnect Alert:** Optional double-pulse wrist vibration if phone connection drops.
- **Status Indicators:** Subtle LED status dots for Bluetooth disconnect and battery low (<20%).

---

## Hardware Gallery & Platform Showcase

<p align="center">
  <img src="screenshots/pulsar-all-platforms-mockup.png" width="100%" alt="All Supported Pebble Platforms" />
</p>

### 1. Pebble Time 2 (`emery`)
* **Hardware:** 316L Brushed Stainless Steel, slim bezel, 1.5" Color Memory LCD (200×228 pixels @ 228 PPI, 64 colors)
* **Highlights:** 53% larger display area, expanded 5×7 LED die spacing, high-definition step bar and status readouts.

<p align="center">
  <img src="screenshots/mockups/pebble-time2-device.png" width="320" alt="Pebble Time 2 Hardware Mockup" />
</p>

| Time (`18:14`) | Live Seconds (`:52`) | Date (`08.21`) | Daily Steps (`08420`) | Battery (`85%`) |
| :---: | :---: | :---: | :---: | :---: |
| ![Emery Time](screenshots/emery-time.png) | ![Emery Seconds](screenshots/emery-seconds.png) | ![Emery Date](screenshots/emery-date.png) | ![Emery Steps](screenshots/emery-steps.png) | ![Emery Battery](screenshots/emery-battery.png) |

---

### 2. Pebble Time & Time Steel (`basalt`)
* **Hardware:** Marine-grade Stainless Steel / Polycarbonate, 1.26" Color Memory LCD (144×168 pixels @ 182 PPI, 64 colors)
* **Highlights:** Deep black glass border integration, rich 64-color palette, full 6-colorway vintage selection.

<p align="center">
  <img src="screenshots/mockups/pebble-basalt-device.png" width="320" alt="Pebble Time Steel Hardware Mockup" />
</p>

| Time (`18:15`) | Live Seconds (`:52`) | Date (`08.21`) | Daily Steps (`08420`) | Battery (`85%`) |
| :---: | :---: | :---: | :---: | :---: |
| ![Basalt Time](screenshots/basalt-time.png) | ![Basalt Seconds](screenshots/basalt-seconds.png) | ![Basalt Date](screenshots/basalt-date.png) | ![Basalt Steps](screenshots/basalt-steps.png) | ![Basalt Battery](screenshots/basalt-battery.png) |

---

### 3. Pebble 2 HR (`diorite`)
* **Hardware:** Matte Sport Polycarbonate with silicone grips, 1.26" Transflective Monochrome Memory LCD (144×168 pixels @ 182 PPI, 1-bit)
* **Highlights:** Glare-free sunlight visibility, pure high-contrast white LED dies, zero dither artifacts.

<p align="center">
  <img src="screenshots/mockups/pebble-diorite-device.png" width="320" alt="Pebble 2 HR Hardware Mockup" />
</p>

| Time (`18:11`) | Live Seconds (`:52`) | Date (`08.21`) | Daily Steps (`08420`) | Battery (`85%`) |
| :---: | :---: | :---: | :---: | :---: |
| ![Diorite Time](screenshots/diorite-time.png) | ![Diorite Seconds](screenshots/diorite-seconds.png) | ![Diorite Date](screenshots/diorite-date.png) | ![Diorite Steps](screenshots/diorite-steps.png) | ![Diorite Battery](screenshots/diorite-battery.png) |

---

### 4. Pebble Classic & Steel (`aplite`)
* **Hardware:** Iconic curved rectangular casing, 1.26" Transflective Monochrome Memory LCD (144×168 pixels @ 182 PPI, 1-bit)
* **Highlights:** Original Pebble architecture support, pixel-aligned dot matrices, ultra-low power consumption.

<p align="center">
  <img src="screenshots/mockups/pebble-aplite-device.png" width="320" alt="Pebble Classic Hardware Mockup" />
</p>

| Time (`18:12`) | Live Seconds (`:52`) | Date (`08.21`) | Daily Steps (`08420`) | Battery (`85%`) |
| :---: | :---: | :---: | :---: | :---: |
| ![Aplite Time](screenshots/aplite-time.png) | ![Aplite Seconds](screenshots/aplite-seconds.png) | ![Aplite Date](screenshots/aplite-date.png) | ![Aplite Steps](screenshots/aplite-steps.png) | ![Aplite Battery](screenshots/aplite-battery.png) |

---

## App Store & Marketing Suite

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

# Simulate wrist flick / tap gesture
devenv shell -- pebble emu-tap --emulator emery --direction x+
```

### 3. Direct Watch Installation (Standard WiFi Developer Bridge)
To flash the watch directly over the local network:
1. Ensure the Pebble mobile app is open on your phone with **Developer Connection** enabled (Settings → Developer Connection).
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

When the Hamilton Watch Company unveiled the original Pulsar in 1970 on the *Tonight Show Starring Johnny Carson*, it had no moving parts, gears, or hands—an unprecedented leap in solid-state electronics. In 1972, Hamilton incorporated the dedicated subsidiary **"Pulsar Time Computer, Inc."** All original P1, P2 ("Astronaut"), and P3 timepieces were branded as *Pulsar Time Computers*.

---

## License

MIT © Travers McInerney
