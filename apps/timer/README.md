# Pulsar Timer (Countdown)

<p align="center">
  <img src="../../screenshots/mockups/pebble-2-device.png" width="40%" alt="Pebble 2 Timer Mockup" />
</p>

A retro digital countdown timer with quick presets, 10-dot depletion progress gauge, and reliable Pebble background wakeup alarms.

## Features

- **Quick Presets:** Instant selection for 1m, 3m, 5m, 10m, 15m, 20m, 25m (Pomodoro), 30m, 45m, and 60m.
- **10-Dot Progress Depletion Gauge:** Unlights micro-LED beads in real time as the timer counts down.
- **Pebble Wakeup API:** Schedules background OS alerts via `wakeup_schedule()`. The watch automatically wakes up, rings, and vibrates even if you leave the app or let the watch sleep.
- **Audible Warning Chimes:** Ascending frequency warning chimes on the final 3 seconds (`3`, `2`, `1`).
- **Flashing Space-Age Alarm:** Flashing ruby red alert display with repeating 8-bit alarm tones and vibration pulses.

## Controls

- **Idle Mode:**
  - **SELECT:** Start Countdown.
  - **UP / DOWN:** Cycle Preset Durations.
- **Running Mode:**
  - **SELECT:** Pause Countdown.
  - **UP:** Quick Add +1 Minute (`+60s`).
  - **DOWN:** Quick Subtract -1 Minute (`-60s`).
- **Paused Mode:**
  - **SELECT:** Resume Countdown.
  - **DOWN:** Reset to initial preset.
- **Alarm Firing Mode:**
  - **Any Button:** Dismiss Alarm.

## Building

```bash
devenv shell -- pebble build
```
