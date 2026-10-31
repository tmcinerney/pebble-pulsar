# Changelog: Pulsar 1970 Watchface

All notable changes to the **Pulsar 1970 Watchface** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [3.0.0] - 2026-10-31

### Added
- **Blink Colon toggle:** Added a setting under *Time & Date* to turn off the 1 Hz blinking colon separator.
- **Dynamic Tick Timer Throttling:** Automatically throttles CPU ticks from `SECOND_UNIT` (60/min) down to `MINUTE_UNIT` (1/min) when the blinking colon is turned off, when Stealth mode is asleep, or when Pebble Health detects the wearer is asleep. This cuts up to 98.3% of daily screen redraws and SPI bus transfers, eliminating extra battery drain over stock watchfaces.
- **Palm-to-Sleep Support:** Fully integrated with PebbleOS 4.41+ palm gestures via `BacklightService`. Covering the display with your palm immediately turns off the backlight and returns the watchface to Time / sleep.
- **Two-Step Flick & Tap Interaction:** The first wrist flick from dark illuminates the backlight and stays on the Time screen (does not jump to Seconds). A second flick or tap during the interaction window pages through secondary screens.
- **LED Glow:** Each lit dot is ringed by a dimmer halo of the same hue, so the matrix reads as a light source rather than a printed dot.
- **Ghost Dots toggle:** The dim unlit dots can now be turned off. Off by default, which noticeably lifts the contrast of the lit digits.
- **Tint Backlight:** Optionally tints the backlight to match the LED colour on supported hardware.
- **Menu icon**, so the watchface is identifiable in the app list.
- **Reset to Defaults** button in settings.

### Changed
- Settings regrouped into LED Appearance, Face Text, Display, Time & Date, Micro-LED Bar, Screens and Alerts, with explanatory text blocks removed and labels shortened.
- Wrist gestures now use the watch's native motion and tap detection: raising your wrist wakes to the time, and subsequent taps page through screens. The legacy 10 Hz continuous accelerometer polling loop has been completely eliminated.
- The watchface no longer forces the backlight on or off. Display illumination follows your Pebble's native motion, ambient light, and timeout settings.
- Colourway names unified across the suite. "Cobalt Blue" is now "Electric Cyan", which is what it actually renders.
- Bluetooth disconnect alerts removed; the firmware already signals this per your notification settings.

### Fixed
- Fixed bug where pressing a button to navigate away while the backlight was lit immediately shut off the backlight. The system timeout now applies naturally.
- Fixed Clay WebView `ReferenceError` on platform capability filtering and fixed "Reset to Defaults" button.
- Fixed Progressive Flow charging animation index overflow on the micro-LED bead row.
- Sound, Hourly Beep and Step Goal Alert existed in settings but were never read by the app. Audio fired from the matching vibration setting instead, so turning the beep off did nothing. All three now work, behind a master sound gate, and respect system Quiet Time and muted speaker state.
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
