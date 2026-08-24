# Changelog: Pulsar Timer (Countdown)

All notable changes to the **Pulsar Timer** watchapp will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
