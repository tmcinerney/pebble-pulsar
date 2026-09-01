# Changelog: Pulsar Alarm (Multi-Schedule Wake Clock)

All notable changes to the **Pulsar Alarm** watchapp will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.1.0] - 2026-09-01

### Added
- **LED Glow**, **Ghost Dots** and **Tint Backlight** settings, matching the rest of the suite.
- **Menu icon** and a **Reset to Defaults** button.

### Changed
- Settings regrouped into LED Appearance, Alerts and Alarm, with shortened labels.
- Colourway names unified with the rest of the suite.

### Fixed
- Paging onto a disabled alarm blanked the display entirely, taking the slot indicator with it. The time now stays visible; the bead row and the [ON]/[OFF] footer show whether that alarm is armed.

---

## [1.0.0] - 2026-08-23

### Added
- Initial release of Pulsar Alarm for Pebble OS.
- 4 Independent multi-alarm configuration slots.
- 4 Repeat schedules (`DAILY`, `WEEKDAYS`, `WEEKENDS`, `ONCE`).
- Continuous Bedside Alarm Loop: 30-second repeating alert with illuminated backlight, piezo synth chimes, vibrating pulses, and high-contrast strobe banners.
- Authentic 9-Minute Vintage Snooze (customizable to 5m, 9m, 10m, 15m via Clay).
- In-app time dial-in edit mode with blinking cursor indicators.
- Pebble `Wakeup API` background scheduler integration.
- 7 Vintage LED colorways and 12° Italic slant support.
- Clay configuration framework integration.
