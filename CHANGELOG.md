# Changelog

All notable changes to the **Pulsar 1970** watchface will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.1.0] - 2026-08-23

### Added
- **Optical Heart Rate Mode (`♥ 72`):** Live BPM pulse rate tracking with procedural GaAsP heart glyph on Pebble 2 HR (`diorite`) and health-capable platforms.
- **Customizable Reorderable Cycle Slots:** 5 configurable tap cycle slots with individual enable/disable controls and loop deduplication.
- **Master Clock Synchronized Charging Animations:** Quartz-locked charging animations (`time_ms()`) with 1970s Cylon ping-pong sweep, Theater Marquee, Progressive Flow, and Heartbeat pulse.
- **Bedside Nightlight Mode:** Keeps backlight illuminated while docked on a charging cable.
- **Step Goal Overdrive & Lap 2 Celebrations:** Dynamic `★ GOAL MET ★` (100%–200%) and `★ 2X GOAL ★` (200%+) celebrations with pulsing surplus beads and victory waves.
- **Delta-Guarded Settings Previews:** Live charging and nightlight previews engage only on active user setting changes.
- **New Release Asset Suite:** Added Heart Rate screenshots across Emery, Basalt, Diorite, and Aplite; updated 6-mode wrist flick animation GIF; refreshed App Store 720x320 banner, store icon, and 4-platform lineup mockups.

### Fixed
- **Firmware Memory & Bounds Safety:** Fixed `cycle_pos` bead flow calculation to prevent integer truncation overshoot.
- **Platform Health Isolation:** Added `#if defined(PBL_HEALTH)` guards preventing invalid state transitions on classic non-health platforms (`aplite`).
- **Pristine Code Formatting:** Cleaned trailing whitespace and normalized line endings across `pulsar.c`.

---

## [2.0.0] - 2026-08-22

### Added
- **Multi-Mode Display Engine:** 5 display modes (Time, Live Seconds, Date, Daily Steps, Battery).
- **Stealth Push-to-Wake (1970 Mode):** 4-second wrist-flick/crystal-tap illumination with automatic backlight activation.
- **Capacitive Touch Support:** Native `TouchService` integration for Pebble Time 2 (`emery`).
- **Retro Synth Speaker Chimes:** 8-bit square/sawtooth audio chirps on speaker-equipped hardware.
- **10-Dot Micro-LED Progress Bar:** Multi-mode indicator above bezel footer.
- **Vintage Colorways:** 6 vintage LED colorways (Neon Ruby, Deep Red, Prototype Green, Amber Gold, Cobalt Blue, Lunar White) plus Inverted Paper mode.
- **12° Vintage Italic Slant:** Toggleable physical rightward die tilt.
- **Rebble Clay Configuration:** Full mobile settings page with persistent local storage.

---

## [1.0.0] - 2026-08-20

### Added
- Initial release of Pulsar 1970 for Pebble.
- Procedural 5×7 GaAsP dot-matrix LED engine.
- 12h/24h time display with AM/PM indicators and ghost matrix background.
- Support for `emery`, `basalt`, `diorite`, and `aplite` platforms.
