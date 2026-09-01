# Changelog: Pulsar Timer (Countdown)

All notable changes to the **Pulsar Timer** watchapp will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.1.0] - 2026-09-01

### Added
- **LED Glow**, **Ghost Dots** and **Tint Backlight** settings, matching the rest of the suite.
- **Menu icon** and a **Reset to Defaults** button.

### Changed
- Settings regrouped into LED Appearance and Alerts, with shortened labels.
- Colourway names unified with the rest of the suite.

### Fixed
- The up and down buttons cycled presets the wrong way: pressing DOWN on a 1:00 preset moved to 2:00.
- Editing minutes or seconds wrapped from 0 up to the maximum, which read as an increment from the timer's starting value of zero. Both now stop at their limits.
- The glow halo blurred the smaller digits, whose dots sit closer together than the main matrix.

---

## [1.0.0] - 2026-08-23

### Added
- Initial release of Pulsar Timer for Pebble OS.
- 15 Quick duration presets including sub-minute intervals (`00:10`, `00:30`, `00:45`), standard intervals (1m to 90m), and Pomodoro (25m).
- Interactive Custom Duration Editor: Long-press SELECT (500ms) to dial in exact minutes (0–99) and seconds (0–59) with blinking LED digits.
- 10-Dot micro-LED progress depletion gauge.
- Pebble `Wakeup API` hardware background scheduling.
- Ascending 3-2-1 warning countdown beeps (Pebble Time 2).
- Continuous 30-Second Alarm Loop: High-visibility strobe banners, illuminated backlight, piezo buzzer chimes, and repeating vibration.
- Quick add/subtract (+15s / +1m) during active countdown.
- 7 Vintage LED colorways and 12° Italic slant support.
- Clay configuration framework integration.
