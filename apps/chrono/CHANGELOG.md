# Changelog: Pulsar Chrono (Stopwatch)

All notable changes to the **Pulsar Chrono** watchapp will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.2.0] - 2026-09-01

### Added
- **LED Glow**, **Ghost Dots** and **Tint Backlight** settings, matching the rest of the suite.
- **Menu icon** and a **Reset to Defaults** button.

### Changed
- Settings regrouped into LED Appearance and Alerts, with shortened labels.
- Removed **Always-On Backlight**. Backlight behaviour is governed by your watch's own motion, ambient and timeout settings.
- Colourway names unified with the rest of the suite.

### Fixed
- Elapsed time compounded across relaunches. Restoring a running stopwatch folded in the time away but left the stored start timestamp untouched, so if the system closed the app without a clean exit the next launch added the same gap again. A 10-second stopwatch could report 35 seconds after five relaunches.
- The glow halo blurred the hundredths sub-dial, whose dots sit closer together than the main matrix.

---

## [1.1.0] - 2026-08-23

### Added
- **Live Lap Split Freeze:** Pressing `UP` during active timing records a lap split and holds the split time frozen on screen for 3 seconds while background timing continues uninterrupted.
- **Interactive Lap Review Browser:** When stopped, pressing `UP` or `DOWN` steps through recorded laps (`LAP 01`, `LAP 02`, etc.) with centisecond split times.
- **Best Lap Delta Indicator:** Fastest recorded lap split is flagged with `[BEST]` in the footer.
- **Automatic Hours Scaling Mode:** Automatically transitions to `HH:MM :SS` format when elapsed time exceeds 60 minutes, with a `CHRONO [HR]` indicator header.
- **Always-On Backlight Setting:** Added toggleable `AppKeyBacklightAlwaysOn` setting via Clay to keep the display illuminated during workouts.
- **Long-Press Reset Confirmation:** Holding `DOWN` for 600ms clears all recorded lap splits from memory and flash storage.
- **Independent Audio & Haptics Gating:** Speaker tones and vibration pulses are now strictly isolated behind their respective Clay toggles.

### Changed
- Refactored `pulsar_audio` integration to take `(bool audio, bool vibe)` flags directly.
- Enhanced font spacing and vertical centering on Pebble Time 2 (`emery`) 200x228 display.

---

## [1.0.0] - 2026-08-20

### Added
- Initial release of Pulsar Chronograph for Pebble OS.
- 50ms high-frequency refresh loop with centisecond precision (`MM:SS.cc`).
- 10-Dot Optical Tachymeter Micro-LED chaser animation.
- Zero-drift epoch persistence across app switches and restarts.
- 7 Vintage LED colorways (GaAsP Ruby Red, Hot Lava, GaP Green, Amber Gold, Cobalt Blue, Lunar White, Inverted Paper).
- Authentic 12° Italic Slant procedural rendering.
- Clay configuration framework integration.
