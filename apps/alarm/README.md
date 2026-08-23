# Pulsar Alarm (Multi-Schedule Wake Clock)

<p align="center">
  <img src="../../screenshots/mockups/pebble-classic-device.png" width="40%" alt="Pebble Classic Alarm Mockup" />
</p>

A multi-schedule digital alarm clock for Pebble OS styled in authentic 1970s GaAsP dot-matrix LEDs, complete with custom repeat patterns and vintage 9-minute snooze handling.

## Features

- **4 Multi-Alarm Slots:** Configure independent alarm slots with individual repeat rules.
- **Repeat Schedules:** `DAILY`, `WEEKDAYS` (Mon–Fri), `WEEKENDS` (Sat–Sun), or `ONCE` (one-shot).
- **Pebble Wakeup API:** Schedules next upcoming trigger timestamp in Pebble OS background scheduler. Automatically launches the alarm app and rings when triggered.
- **Escalating Alarm Wave:** Pulsating 8-bit synth audio chirps and high-energy haptic vibration.
- **Vintage Snooze Support:** Built-in 9-minute snooze timer (configurable to 5m, 9m, 10m, or 15m in Clay settings).
- **Intuitive Time Dial-In:** Set hours, minutes, and repeat rules directly on the watch.

## Controls

- **Normal Mode:**
  - **UP / DOWN:** Switch between Alarm Slots (1 through 4).
  - **SELECT (Short Press):** Toggle active alarm `[ ON ]` / `[ OFF ]`.
  - **SELECT (Long Press / Hold):** Enter Edit Mode.
- **Edit Mode:**
  - **UP / DOWN:** Increment / Decrement active field.
  - **SELECT:** Advance field (Hour $\rightarrow$ Minute $\rightarrow$ Repeat $\rightarrow$ Save & Exit).
- **Ringing Alarm Mode:**
  - **UP / DOWN / SELECT (Short):** Snooze alarm (+9 minutes).
  - **SELECT (Long Press / Hold):** Dismiss alarm completely.

## Building

```bash
devenv shell -- pebble build
```
