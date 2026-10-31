# Pulsar 1970 v3.0.0 Release Notes

**Release Date:** October 2026  
**Target Platforms:** Pebble Time 2 (`emery`), Pebble Time / Time Steel (`basalt`), Pebble 2 HR (`diorite`), Pebble Classic & Steel (`aplite`)

---

## 🚀 Overview

**Pulsar 1970 v3.0.0** ("The Battery & Native Performance Update") is a major architectural overhaul that slashes CPU active time and display bus traffic by up to 98%, completely eliminates background accelerometer polling in favor of native PebbleOS 4.41+ system services, adds the highly-requested toggleable solid colon, introduces procedural optical LED glow halos, and brings full native touch navigation to the companion apps.

---

## 🌟 What's New in v3.0.0

### 1. Dynamic Tick Throttling & Battery Saver Colon Mode
- **Blink Colon Toggle:** A new setting under *Time & Date* allows turning off the 1 Hz colon pulse, keeping the `:` separator solid.
- **Dynamic `SECOND_UNIT` ↔ `MINUTE_UNIT` Throttling:** When the colon is static, in stealth sleep, or when Pebble Health detects the wearer is asleep, the watchface drops from 60 CPU wakeups per minute down to 1 per minute.
- **Direct Battery Solution:** Eliminates over 84,960 full-screen matrix redraws and SPI bus transfers per day, resolving the +241 µA draw measured on Pebble Time 2.

### 2. Native PebbleOS Gesture & Backlight Integration
- **Two-Step Flick/Tap Interaction:** The first wrist flick from dark illuminates the backlight and stays locked on the Time screen (does not jump to Seconds). A second flick or tap during the interaction window pages secondary screens.
- **Palm-to-Sleep:** Fully integrated with PebbleOS 4.41+ palm gestures via `BacklightService` to immediately sleep and return to Time.
- **Backlight Persistence Fix:** Exiting the watchface or companion apps on a button press no longer cuts off the backlight prematurely; the system timeout applies naturally.

### 3. Procedural LED Glow & Ghost Dots
- **Optical Glow Halos:** Each lit diode is surrounded by a calculated bloom halo of the same hue, accurately reproducing the visual weight of real GaAsP solid-state LED dies.
- **Ghost Dots Toggle:** The unlit 5×7 diode matrix can now be toggled on or off (off by default), providing stark, high-contrast visibility on reflective e-paper screens.
- **RGB Backlight Tinting:** Optionally matches the watch backlight LED to your selected colourway on supported hardware.

### 4. Platform Respect & Audio Modernization
- **Quiet Time & Speaker Mute:** Hourly beeps, chimes, and step celebrations automatically respect PebbleOS Quiet Time (`quiet_time_is_active()`) and muted speaker states (`speaker_is_muted()`).
- **Native Touch Navigation:** Companion apps (`chrono`, `timer`, `alarm`) now participate in native PebbleOS touch navigation via `app_touch_navigation_enable(true)`.

---

# Pulsar 1970 v2.1.0 Release Notes

**Release Date:** August 2026  
**Target Platforms:** Pebble Time 2 (`emery`), Pebble Time / Time Steel (`basalt`), Pebble 2 HR (`diorite`), Pebble Classic & Steel (`aplite`)

---

## 🚀 Overview

**Pulsar 1970 v2.1.0** ("The Interactive Health & Retro Charging Update") is a major feature update that introduces optical heart-rate monitoring, completely customizable tap cycle ordering, quartz-synchronized 1970s retro charging animations, bedside nightlight mode, and step goal overdrive celebrations across the entire Pebble family.

---

## 🌟 What's New in v2.1.0

### 1. Optical Heart Rate Mode (`♥ 72`)
- **Procedural GaAsP Heart Glyph:** Features an authentic 5×7 dot-matrix heart icon alongside real-time BPM pulse readouts.
- **Pebble Health Sensor Integration:** Reads directly from the watch's optical heart-rate sensor on Pebble 2 HR (`diorite`) and compatible health platforms.
- **Graceful Fallbacks & Offline Dash Matrix (`♥ --`):** Displays a clean dashed matrix while the sensor acquires a lock or when heart rate data is unavailable.
- **Hardware-Guarded Architecture:** Guarded under `#if defined(PBL_HEALTH)` to ensure rock-solid stability and zero overhead on classic non-health platforms (`aplite`).

### 2. Fully Customizable & Reorderable Tap Cycle
- **5 Configurable Sequence Slots:** Reorder exactly which screens appear when flicking your wrist, tapping the crystal, or touching the screen (Pebble Time 2).
- **Individual Toggle Control:** Set any slot to *Disabled / None* to skip modes you don't use.
- **Loop Trap Deduplication:** Built-in hardware safeguard prevents duplicate screen selections from trapping navigation.

### 3. Quartz Master-Synchronized Charging Animations
- **Millisecond-Locked Precision:** All charging animations are locked to master whole seconds and sub-second millisecond intervals via `time_ms()`.
- **4 Authentic Retro Styles:**
  - **1970s Cylon / Knight Rider Chaser:** 3-dot ping-pong comet sweep across the 10 micro-LEDs (2.0s period).
  - **1970s Theater Marquee:** Alternating odd/even micro-LEDs pulsing on 500ms half-second intervals.
  - **Progressive Flow / Fill:** Cascading LED flow across uncharged beads resetting on whole seconds.
  - **Breathing / Heartbeat Pulse:** Dual-beat heartbeat pulse (`lub-dub`) synchronized to the second colon.
  - **Solid Gauge:** Minimalist static power meter.
- **Delta-Guarded Settings Preview:** 12-second live preview engages in the emulator or on your wrist only when you actively modify the setting in Clay.

### 4. Bedside Nightlight Mode
- **Continuous Nightstand Illumination:** Keeps the screen backlight continuously active while docked on a charging cable or bedside nightstand.
- **5-Second Quick Preview:** Activates a 5-second preview when toggled in the Pebble mobile settings app.

### 5. Step Goal Overdrive & Lap 2 Celebrations
- **Goal Met (100%–200%):** Celebratory footer displays `★ GOAL MET ★` with surplus "Lap 2" progress beads pulsing rhythmically each second.
- **200%+ Overdrive:** Footer shifts to `★ 2X GOAL ★` accompanied by a synchronized victory wave across all 10 micro-LED beads.

---

## 🛠️ Improvements & Fixes
- **Firmware Memory & Bounds Safety:** Fixed `cycle_pos` progressive bead flow calculations to eliminate overflow conditions.
- **Refreshed Asset Suite:** Updated marketing materials, App Store 720×320 banner, 4-platform hardware mockup lineup, and 6-mode wrist-flick animated GIF.
- **Verified Cross-Platform Compatibility:** 100% test pass rate across `aplite`, `basalt`, `diorite`, and `emery` test targets.

---

## 📦 Distribution & Installation
- **Rebble App Store:** Search for **"Pulsar 1970"** or install directly from `build/pebble-pulsar.pbw`.
- **Developer Direct Install:** `devenv shell -- python3 scripts/install_watch.py <PHONE_IP>`
