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
