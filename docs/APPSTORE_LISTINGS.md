# Rebble Appstore Distribution Materials

This document provides ready-to-copy listings, metadata, release notes, and image asset mappings for each application in the **Pebble Pulsar Suite**.

---

# 1. Pulsar 1970 Watchface `v2.2.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar 1970` |
| **Category** | `Watchfaces` |
| **Type** | `Watchface` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `2.2.0` |
| **Source Code URL** | `https://github.com/tmcinerney/pebble-pulsar` |
| **Target Architectures** | `Emery (Pebble Time 2)`, `Basalt (Pebble Time/Steel)`, `Diorite (Pebble 2 HR)`, `Aplite (Pebble Classic)` |

---

### 📝 Short Subtitle / Teaser
```
The authentic 1972 Hamilton Pulsar Time Computer LED watchface with procedural dot-matrix graphics, Health tracking, and 8-bit audio synth chimes.
```

---

### 📄 Full Store Description (Copy & Paste)
```markdown
Step into 1970s horological history with **Pulsar 1970**, a faithful recreation of the world's first commercial digital LED wristwatch — the Hamilton Pulsar Time Computer.

Powered by a lightweight procedural C graphics engine (`libpulsar`), Pulsar renders authentic 5x7 GaAsP circular dot-matrix LEDs with sub-surface phosphor glow and 12° vintage italic slant.

### ⚡ Operating Modes
- **Time Mode:** Crisp `HH:MM` time with optional leading zero and customizable header/footer.
- **Seconds Mode:** Live ticking `:SS` display.
- **Date Mode:** Full `MM.DD` calendar display with day-of-week indicator.
- **Pebble Health Steps Mode:** Live step count with daily goal tracking.
- **Battery Mode:** Exact battery percentage (`BATT`) with charge state.
- **Heart Rate Mode:** Real-time pulse monitoring (`BPM`) on supported hardware.

### 🌟 Key Features
- **Pebble Health 10-Dot Progress Bar:** Micro-LED bead gauge tracking daily step targets with Overdrive celebration animation upon reaching 100%.
- **Wrist Flick / Tap Gesture Cycling:** Seamlessly switch modes with a gentle wrist flick or button tap.
- **Clock-Synchronized Charging Animations:** Cylon eye scanner, theater marquee, and heartbeat pulse while on the charger.
- **Bedside Nightlight:** Keeps the screen illuminated while charging on your nightstand.
- **8-Bit Retro Synth Audio:** Hourly tone chimes, step celebration fanfares, and Bluetooth connection alerts (Pebble Time 2).
- **7 Vintage Colorways:** GaAsP Ruby Red, Hot Lava Orange, GaP Phosphor Green, Amber Gold (HP-01), Cobalt Blue, Lunar White, and Inverted High-Contrast Paper.
```

---

### 🚀 Release Notes (What's New in v2.2.0)
```markdown
- Added 8-bit retro synth audio chimes for hourly alerts, step goal fanfares, and Bluetooth disconnection (Pebble Time 2).
- Isolated audio sound effects and haptic vibration toggles into independent settings.
- Enhanced bedtime nightlight mode with automatic backlight holding while connected to charger.
- Optimized 12° italic slant font cache for ultra-low battery consumption.
```

---

### 🖼️ Asset Upload Mapping for Pulsar 1970

| App Store Asset | Source File in Monorepo | Dimensions |
| :--- | :--- | :--- |
| **Appstore Banner** | `dist/appstore-submission/watchface/banner-watchface-720x320.png` | `720 x 320` |
| **Store Icon / Tile** | `dist/appstore-submission/watchface/icon-watchface-260x260.png` | `260 x 260` |
| **Pebble Time 2 Screenshot** | `dist/appstore-submission/watchface/screenshot-emery.png` | `200 x 228` |
| **Pebble Time Steel Screenshot** | `dist/appstore-submission/watchface/screenshot-basalt.png` | `144 x 168` |
| **Pebble 2 HR Screenshot** | `dist/appstore-submission/watchface/screenshot-diorite.png` | `144 x 168` |
| **Pebble Classic Screenshot** | `dist/appstore-submission/watchface/screenshot-aplite.png` | `144 x 168` |
| **Binary Package (PBW)** | `dist/appstore-submission/watchface/pebble-pulsar-watchface.pbw` | Target `.pbw` |

---
---

# 2. Pulsar Chrono (Precision Stopwatch) `v1.1.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar Chrono` |
| **Category** | `Tools & Utilities` (or `Sports & Fitness`) |
| **Type** | `Watchapp` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `1.1.0` |
| **Source Code URL** | `https://github.com/tmcinerney/pebble-pulsar` |
| **Rebble Store URL** | [**`apps.rebble.io/application/6a8bc61f06d13300097edc49`**](https://apps.rebble.io/en_US/application/6a8bc61f06d13300097edc49) |
| **Target Architectures** | `Emery`, `Basalt`, `Diorite`, `Aplite` |

---

### 📝 Short Subtitle / Teaser
```
Precision 1970s digital LED chronograph with 50ms centisecond readout, live lap split freeze, interactive 20-lap browser, and optical tachymeter.
```

---

### 📄 Full Store Description (Copy & Paste)
```markdown
**Pulsar Chrono** is a high-performance digital chronograph and split-lap stopwatch for Pebble OS styled after vintage 1970s solid-state LED timing instruments.

### ⚡ Timing Features
- **Centisecond Precision:** High-rate 50ms (20fps) refresh loop displaying split-second timing in `MM:SS.cc`.
- **Automatic Hours Scaling:** Seamlessly switches to `HH:MM :SS` format when timing passes 60 minutes with `CHRONO [HR]` indicator.
- **Live Lap Split Freeze:** Pressing `UP` during active timing records a lap split (up to 20 laps) and freezes the split time on-screen for 3 seconds while background timing continues unaffected.
- **Interactive Lap Review Browser:** When stopped, press `UP` or `DOWN` to cycle through recorded splits (`LAP 01`, `LAP 02`, etc.) with the fastest split highlighted as `[BEST]`.
- **10-Dot Micro-LED Tachymeter:** Smooth optical bead chaser animating split-second progression.
- **Zero-Drift Background Persistence:** Exiting to your watchface or switching apps preserves elapsed time and running state via epoch timestamps with zero background battery drain.
- **Always-On Backlight Mode:** Toggleable backlight hold to keep the screen readable during workouts.
- **Retro Synth Audio & Tactile Haptics:** Distinct actuation buzzer tones and haptic pulses.

### 🎮 Hardware Controls
- **SELECT:** Start / Stop timing loop.
- **UP (Running):** Record Lap Split (holds split time on-screen for 3s).
- **UP / DOWN (Stopped):** Browse recorded lap splits with centisecond accuracy.
- **DOWN (Stopped at Reset):** Reset to `00:00.00`.
- **DOWN (Hold 600ms):** Clear all stored laps from memory.
- **BACK:** Return to watchface (timing continues safely in background).
```

---

### 🚀 Release Notes (What's New in v1.1.0)
```markdown
- Added Live Lap Split Freeze: Pressing UP during active timing holds the split time on screen for 3 seconds while background timing continues uninterrupted.
- Added Interactive Lap Review Browser: Step through up to 20 recorded laps with centisecond split readouts when stopped.
- Added Best Lap Delta Indicator: Automatically highlights your fastest recorded lap as [BEST].
- Added Automatic Hours Scaling Mode: Transitions to HH:MM :SS format past 60 minutes.
- Added Always-On Backlight Setting via Clay.
- Strictly isolated speaker audio tones and haptic vibrations into independent toggles.
```

---

### 🖼️ Asset Upload Mapping for Pulsar Chrono

| App Store Asset | Source File in Monorepo | Dimensions |
| :--- | :--- | :--- |
| **Appstore Banner** | `dist/appstore-submission/chrono/banner-chrono-720x320.png` | `720 x 320` |
| **Store Icon / Tile** | `dist/appstore-submission/chrono/icon-chrono-260x260.png` | `260 x 260` |
| **Pebble Time 2 Screenshot** | `dist/appstore-submission/chrono/screenshot-emery.png` | `200 x 228` |
| **Pebble Time Steel Screenshot** | `dist/appstore-submission/chrono/screenshot-basalt.png` | `144 x 168` |
| **Pebble 2 HR Screenshot** | `dist/appstore-submission/chrono/screenshot-diorite.png` | `144 x 168` |
| **Pebble Classic Screenshot** | `dist/appstore-submission/chrono/screenshot-aplite.png` | `144 x 168` |
| **Binary Package (PBW)** | `dist/appstore-submission/chrono/pebble-pulsar-chrono.pbw` | Target `.pbw` |

---
---

# 3. Pulsar Timer (Countdown & Pomodoro) `v1.0.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar Timer` |
| **Category** | `Tools & Utilities` (or `Daily`) |
| **Type** | `Watchapp` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `1.0.0` |
| **Source Code URL** | `https://github.com/tmcinerney/pebble-pulsar` |
| **Target Architectures** | `Emery`, `Basalt`, `Diorite`, `Aplite` |

---

### 📝 Short Subtitle / Teaser
```
Vintage 1970s solid-state LED countdown timer and Pomodoro focus tool with background wake alerts and 10-dot depletion gauge.
```

---

### 📄 Full Store Description (Copy & Paste)
```markdown
**Pulsar Timer** brings the tactile retro charm of 1970s digital LED instrumentation to interval timing and focus management.

### ⚡ Features
- **15 Quick Duration Presets:** Sub-minute intervals (`00:10`, `00:30`, `00:45`), standard intervals (1m to 90m), and Pomodoro (25m).
- **Interactive Custom Duration Editor:** Long-press SELECT (500ms) to dial in exact custom minutes (0–99) and seconds (0–59) with blinking LED digits.
- **10-Dot Micro-LED Depletion Gauge:** Progressive optical bead gauge visually displaying remaining time proportion.
- **Continuous 30-Second Bedside Alarm Loop:** Flashing strobe banners, illuminated backlight, buzzer piezo synth chimes, and repeating vibration.
- **Pebble Wakeup API:** Reliable background alarms wake the watch and fire alerts even if you switch apps or turn off the screen.
- **Ascending 3-2-1 Audio Warnings:** Warning buzzer beeps at 3s, 2s, 1s leading into timer expiration.
- **Live Time Adjustment:** Add or subtract time (+15s / +1m) during active countdowns.

### 🎮 Hardware Controls
- **UP / DOWN (Preset Selection):** Choose preset duration.
- **SELECT (Preset Selection):** Start countdown timer.
- **SELECT (Hold from Preset Picker):** Enter Custom Duration Editor (dial in exact MM:SS).
- **UP / DOWN (Running):** Quick adjust remaining time (+1m / -1m or +15s / -15s).
- **SELECT (Running):** Pause / Resume countdown.
- **SELECT (Hold while Paused):** Reset timer to preset picker.
- **ANY BUTTON (Alarm Firing):** Dismiss alarm alert.
```

---

### 🚀 Release Notes (What's New in v1.0.0)
```markdown
- Initial release of Pulsar Timer for Pebble OS.
- 15 Quick duration presets including sub-minute intervals (10s, 30s, 45s) and 25m Pomodoro.
- Interactive Custom Duration Editor for exact minute and second settings.
- 10-Dot micro-LED progress depletion gauge.
- Continuous 30-second alarm firing loop with glowing backlight, piezo tones, and repeating haptics.
- Pebble Wakeup API background scheduler integration.
- Ascending 3-2-1 audio warning ticks and ruby alarm alert screen.
- 7 Vintage LED colorways and 12° Italic slant support.
```

---

### 🖼️ Asset Upload Mapping for Pulsar Timer

| App Store Asset | Source File in Monorepo | Dimensions |
| :--- | :--- | :--- |
| **Appstore Banner** | `dist/appstore-submission/timer/banner-timer-720x320.png` | `720 x 320` |
| **Store Icon / Tile** | `dist/appstore-submission/timer/icon-timer-260x260.png` | `260 x 260` |
| **Pebble Time 2 Screenshot** | `dist/appstore-submission/timer/screenshot-emery.png` | `200 x 228` |
| **Pebble Time Steel Screenshot** | `dist/appstore-submission/timer/screenshot-basalt.png` | `144 x 168` |
| **Pebble 2 HR Screenshot** | `dist/appstore-submission/timer/screenshot-diorite.png` | `144 x 168` |
| **Pebble Classic Screenshot** | `dist/appstore-submission/timer/screenshot-aplite.png` | `144 x 168` |
| **Binary Package (PBW)** | `dist/appstore-submission/timer/pebble-pulsar-timer.pbw` | Target `.pbw` |

---
---

# 4. Pulsar Alarm (Multi-Schedule Clock) `v1.0.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar Alarm` |
| **Category** | `Daily` (or `Tools & Utilities`) |
| **Type** | `Watchapp` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `1.0.0` |
| **Source Code URL** | `https://github.com/tmcinerney/pebble-pulsar` |
| **Target Architectures** | `Emery`, `Basalt`, `Diorite`, `Aplite` |

---

### 📝 Short Subtitle / Teaser
```
Vintage multi-schedule digital LED alarm clock with 4 independent alarm slots, flexible repeat rules, bedside strobe mode, and 9-minute snooze.
```

---

### 📄 Full Store Description (Copy & Paste)
```markdown
**Pulsar Alarm** transforms your Pebble into an authentic 1970s solid-state digital alarm clock with multi-alarm scheduling and vintage snooze behavior.

### ⚡ Features
- **4 Independent Alarm Slots:** Configure up to 4 discrete alarms with individual wake times and schedules.
- **4 Flexible Repeat Schedules:** Choose between `DAILY`, `WEEKDAYS` (Mon-Fri), `WEEKENDS` (Sat-Sun), and `ONCE`.
- **Bedside Clock Alarm Mode:** Leave open on your nightstand for continuous 30-second repeating vibration pulses, retro piezo chimes, illuminated backlight, and flashing alert banners.
- **Pebble Wakeup API Engine:** Reliable background execution wakes the watch and sounds alarms even from deep standby.
- **Authentic 9-Minute Snooze:** Traditional mechanical snooze duration (customizable to 5m, 9m, 10m, or 15m via Clay).
- **Interactive Dial-In Edit Mode:** Dial in hours, minutes, and repeat rules directly on your wrist with blinking cursor feedback.
- **Escalating Synth & Haptic Curve:** Gentle initial tone pulse escalating into high-urgency wake alarms.

### 🎮 Hardware Controls
- **UP / DOWN (Slot List):** Navigate between Alarm Slots 1–4.
- **SELECT (Slot List):** Toggle Selected Alarm On / Off.
- **SELECT (Hold):** Enter Edit Mode for selected alarm.
- **UP / DOWN (Edit Mode):** Adjust active value (Hours / Minutes / Schedule).
- **SELECT (Edit Mode):** Advance cursor / Confirm settings.
- **SELECT (Firing Alarm):** Snooze alarm (+9 minutes).
- **BACK / DOWN (Firing Alarm):** Dismiss alarm.
```

---

### 🚀 Release Notes (What's New in v1.0.0)
```markdown
- Initial release of Pulsar Alarm for Pebble OS.
- 4 Independent multi-alarm configuration slots.
- Flexible repeat schedules (Daily, Weekdays, Weekends, Once).
- Continuous Bedside Alarm Mode with illuminated backlight, piezo synth chimes, and strobe banners.
- Pebble Wakeup API background scheduler integration.
- Vintage 9-minute snooze and in-app dial-in edit mode.
- 7 Vintage LED colorways and 12° Italic slant support.
```

---

### 🖼️ Asset Upload Mapping for Pulsar Alarm

| App Store Asset | Source File in Monorepo | Dimensions |
| :--- | :--- | :--- |
| **Appstore Banner** | `dist/appstore-submission/alarm/banner-alarm-720x320.png` | `720 x 320` |
| **Store Icon / Tile** | `dist/appstore-submission/alarm/icon-alarm-260x260.png` | `260 x 260` |
| **Pebble Time 2 Screenshot** | `dist/appstore-submission/alarm/screenshot-emery.png` | `200 x 228` |
| **Pebble Time Steel Screenshot** | `dist/appstore-submission/alarm/screenshot-basalt.png` | `144 x 168` |
| **Pebble 2 HR Screenshot** | `dist/appstore-submission/alarm/screenshot-diorite.png` | `144 x 168` |
| **Pebble Classic Screenshot** | `dist/appstore-submission/alarm/screenshot-aplite.png` | `144 x 168` |
| **Binary Package (PBW)** | `dist/appstore-submission/alarm/pebble-pulsar-alarm.pbw` | Target `.pbw` |
