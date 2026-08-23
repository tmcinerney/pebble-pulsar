# Changelog: Pulsar 1970 Watchface

All notable changes to the **Pulsar 1970 Watchface** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.2.0] - 2026-08-23

### Added
- **Retro 8-bit Synth Audio Chimes:** Added optional hourly tone chimes, step goal milestone fanfares, and Bluetooth connection alerts on hardware with speaker support (Pebble Time 2 `emery`).
- **Independent Audio & Haptics:** Discrete Clay toggles for sound effects vs haptic vibration.
- **Enhanced Bedside Nightlight Mode:** Automatic backlight holding when placed on charger stand.

### Changed
- Refactored audio calls to pass explicit `(bool audio, bool vibe)` arguments.
- Optimized 12° Italic slant font rendering cache for lower battery consumption.

---

## [2.1.0] - 2026-08-15

### Added
- Clock-synchronized charging animations (Cylon chaser, Theater Marquee, Heartbeat pulse).
- Pebble Health 10-Dot Progress Bar with Step Overdrive celebrations.
- 6 Display Modes with configurable flick/tap cycling.

---

## [2.0.0] - 2026-08-01

### Added
- Complete rewrite with procedural 5x7 GaAsP dot-matrix LED graphics engine (`libpulsar`).
- Full support for Pebble Time 2 (`emery`), Pebble Time (`basalt`), Pebble 2 (`diorite`), and Classic (`aplite`).
- 7 Vintage LED colorways.
