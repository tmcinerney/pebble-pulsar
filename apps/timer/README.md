# Pulsar Countdown Timer `v1.0.0`

<p align="center">
  <img src="../../screenshots/mockups/timer/pebble-time2-timer.png" width="38%" alt="Pebble Time 2 Timer Mockup" />
  <img src="../../screenshots/mockups/timer/pebble-basalt-timer.png" width="38%" alt="Pebble Time Steel Timer Mockup" />
</p>

A vintage LED countdown timer and Pomodoro focus tool for Pebble OS styled after the 1970s Hamilton Pulsar digital instruments.

---

## ⚡ Features

- **10 Quick Duration Presets:** Instant selection between 1m, 3m, 5m, 10m, 15m, 20m, 25m (Pomodoro), 30m, 45m, and 60m.
- **10-Dot Micro-LED Depletion Gauge:** Progressive optical bead depletion visualizing remaining countdown proportion.
- **Pebble Wakeup API Integration:** Safely fires alarm notifications and wakes the watch even if you exit the app or the screen turns off.
- **Ascending 3-2-1 Audio Warning:** Warning audio beeps at 3s, 2s, 1s leading into timer completion.
- **Escalating Ruby Alarm Pulse:** Pulsing red LED display alert with buzzer synth tones and haptic vibration pattern.
- **Quick Time Adjust:** Add or subtract 1 minute on the fly during active countdowns.

---

## 🎮 Hardware Controls

| Button | Mode | Action |
| :--- | :--- | :--- |
| **UP / DOWN** | Preset Select | Cycle duration presets (1m to 60m) |
| **SELECT** | Preset Select | **Start** countdown timer |
| **UP / DOWN** | Running | **+1m / -1m** time adjustment |
| **SELECT** | Running | **Pause / Resume** countdown |
| **SELECT (Hold)** | Paused | **Reset** timer to preset selection |
| **ANY BUTTON** | Firing Alarm | **Dismiss** alarm alert |

---

## ⚙️ Clay Configuration Keys

| Key | Description | Type | Default |
| :--- | :--- | :--- | :--- |
| `AppKeyColorway` | LED Palette (Ruby, Lava, Green, Gold, Blue, Lunar, Paper) | select | GaAsP Ruby Red (`0`) |
| `AppKeyItalicSlant` | 12° Italic Display Slant | toggle | Enabled (`true`) |
| `AppKeyAudioEnabled` | Buzzer countdown and alarm tones (Pebble Time 2) | toggle | Enabled (`true`) |
| `AppKeyVibeEnabled` | Haptic actuation and alarm vibration | toggle | Enabled (`true`) |

---

## 📦 Building & Asset Commands

```bash
# Build app PBW bundle
npm run build:timer

# Capture and validate screenshots + generate 3D hardware mockups
npm run assets:timer
```
