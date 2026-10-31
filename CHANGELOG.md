# Changelog

All notable changes to the **Pulsar 1970** watchface will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [3.0.0] - 2026-10-31

### Added
- **Pulsar 1970 v3.0.0 Release:** Major architectural and battery optimization release across the suite.
- **Dynamic Tick Timer Throttling:** Automatically throttles CPU ticks from `SECOND_UNIT` (60/min) down to `MINUTE_UNIT` (1/min) when the blinking colon is turned off, in Stealth sleep, or when Pebble Health detects sleep. Eliminates up to 98.3% of daily screen redraws and SPI bus transfers.
- **Blink Colon Setting:** Added toggleable `AppKeyBlinkColon` under *Time & Date* to turn off the 1 Hz colon pulse for maximum battery life.
- **Two-Step Gesture Interaction:** First wrist flick illuminates the backlight and stays on Time; subsequent flicks or taps during the interaction window page secondary screens.
- **Palm-to-Sleep Support:** Full integration with PebbleOS 4.41+ palm gestures via `BacklightService`.
- **Procedural LED Glow:** Realistic optical halo bloom around lit LEDs.
- **Ghost Dots Toggle:** High-contrast unlit matrix toggle (off by default).
- **RGB Backlight Tinting:** Optionally tints Pebble Time 2 / Flint backlight to match the selected LED colourway.
- **Native Touch Navigation:** Companion watchapps (`chrono`, `timer`, `alarm`) now participate in native PebbleOS touch navigation via `app_touch_navigation_enable(true)`.

### Fixed
- Fixed bug where pressing a button to exit the watchface or companion apps extinguished the backlight prematurely.
- Fixed 32-bit integer overflow in steady-backlight timing calculation.
- Fixed Clay WebView `ReferenceError` during platform capability filtering and fixed "Reset to Defaults" button across all apps.
- Fixed Progressive Flow charging animation index overflow on the micro-LED bead row.
- Audio and vibration settings now function independently across all apps, respecting system Quiet Time and muted speaker state.

---

## [2.2.0] - 2026-08-23

### Added
- **Retro 8-Bit Synth Audio Chimes:** Optional hourly tone chimes, step goal milestone fanfares, and Bluetooth alerts on hardware with speaker support (Pebble Time 2 `emery`).
- **Independent Audio & Haptics:** Discrete Clay settings for sound effects vs haptic vibration.
- **Enhanced Bedside Nightlight Mode:** Automatic backlight holding when placed on a charger stand.
- **Multi-Platform Support:** Pebble Time 2 (`emery`), Pebble Time/Steel (`basalt`), Pebble 2 (`diorite`), and Pebble Classic (`aplite`).

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
