# Changelog: Pulsar 1970 Watchface

All notable changes to the **Pulsar 1970 Watchface** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.3.0] - 2026-09-01

### Added
- **LED Glow:** Each lit dot is ringed by a dimmer halo of the same hue, so the matrix reads as a light source rather than a printed dot.
- **Ghost Dots toggle:** The dim unlit dots can now be turned off. Off by default, which noticeably lifts the contrast of the lit digits.
- **Tint Backlight:** Optionally tints the backlight to match the LED colour. Off by default, since the watch has its own backlight colour setting.
- **Menu icon**, so the watchface is identifiable in the app list.
- **Reset to Defaults** button in settings.

### Changed
- Settings regrouped into LED Appearance, Face Text, Display, Time & Date, Steps, Screens and Alerts, with the explanatory text blocks removed and labels shortened.
- Wrist gestures now use the watch's own motion detection: raising your wrist wakes to the time, and a tap pages through screens. The app-level flick mode and sensitivity settings are gone, along with the accelerometer subscription they required.
- The watchface no longer forces the backlight on or off. When it lights is governed by your watch's motion, ambient and timeout settings.
- Colourway names unified across the suite. "Cobalt Blue" is now "Electric Cyan", which is what it actually renders.
- Bluetooth disconnect alerts removed; the firmware already signals this per your notification settings.

### Fixed
- Sound, Hourly Beep and Step Goal Alert existed in settings but were never read by the app. Audio fired from the matching vibration setting instead, so turning the beep off did nothing. All three now work, behind a master sound gate.
- Step Goal Alert is implemented rather than merely stored, and fires once per day.
- The micro-LED bead row ignored the selected colourway and was always white.
- Stealth mode could be selected on watches with no way to wake the display, leaving it dark with no recovery.
- Stealth mode showed a bare black screen once ghost dots were disabled.

### Removed
- **LED Brightness.** Measured on the panel it raised contrast from 5.7:1 to 12.0:1 but was not perceptible indoors, while cutting colour saturation from 63% to 24%. The Lunar White colourway reaches 21:1 and is the better choice when legibility matters more than authenticity.

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
