# Rebble Appstore Distribution Materials

This document provides ready-to-copy listings, metadata, release notes, and image asset mappings for each application in the **Pebble Pulsar Suite**.

---

# 1. Pulsar 1970 Watchface `v2.3.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar 1970` |
| **Category** | `Watchfaces` |
| **Type** | `Watchface` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `2.3.0` |
| **Source Code URL** | `https://github.com/tmcinerney/pebble-pulsar` |
| **Target Architectures** | `Emery (Pebble Time 2)`, `Basalt (Pebble Time/Steel)`, `Diorite (Pebble 2 HR)`, `Aplite (Pebble Classic)` |

---

### 📝 Short Subtitle / Teaser
```
A dot-matrix LED watchface modelled on the Hamilton Pulsar, the first electronic digital wristwatch.
```

---

### 📄 Full Store Description (Copy & Paste)
```markdown
Hamilton showed the Pulsar "Wrist Computer" as a prototype in May 1970, and sold the P1 from 1972. It was the first watch to tell the time with light-emitting diodes: the display stayed dark until you pressed a button, then lit red digits for a few seconds.

This watchface reproduces that display on the Pebble. Digits are drawn as individual round LEDs on a 5x7 grid, unlit by default, with an optional slight rightward slant and a glow around each lit dot.

### Display modes
- **Time** - `HH:MM`, with optional leading zero
- **Seconds** - live `:SS`
- **Date** - `MM.DD`
- **Steps** - daily step count, where Pebble Health is available
- **Battery** - charge percentage
- **Heart rate** - BPM, on watches with the sensor

Raise your wrist to see the time. Tap the watch to page through the other screens, which return to the time after four seconds. Which screens appear, and in what order, is configurable.

### Other features
- **Micro-LED bar** - a row of ten dots showing progress toward your step goal, or battery level
- **Charging animations** - the bar animates while the watch is charging
- **Seven colourways** - ruby red, hot lava orange, phosphor green, amber gold, electric cyan, lunar white, and a dark-on-light inverted scheme
- **Hourly chime** - optional vibration, and a tone on watches with a speaker
- **Step goal alert** - fires once when you pass your daily goal

The watchface does not control the backlight. When the screen lights, and in what colour, follows your watch's own settings, though you can optionally tint the backlight to match the LED colour.
```

---

### Release notes (v2.3.0)
```markdown
- Added a glow around each lit dot, and a setting to turn off the dim unlit dots. Unlit dots are now hidden by default, which raises the contrast of the digits.
- Wrist gestures now use the watch's own motion detection. Raising your wrist shows the time; tapping pages through the other screens.
- The watchface no longer overrides the backlight. When it lights follows your watch settings; the backlight can optionally be tinted to match the LED colour.
- Settings regrouped and relabelled.
- Fixed: the sound settings did nothing, and the hourly beep played whenever hourly vibration was on.
- Fixed: the step goal alert was never implemented.
- Fixed: the micro-LED bar ignored the selected colourway.
- Removed the LED brightness setting; measured on the panel it was not perceptibly brighter and cost most of the colour saturation.
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

# 2. Pulsar Chrono (Precision Stopwatch) `v1.2.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar Chrono` |
| **Category** | `Tools & Utilities` (or `Sports & Fitness`) |
| **Type** | `Watchapp` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `1.2.0` |
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
**Pulsar Chrono** is a stopwatch drawn in the same LED dot-matrix style as the Pulsar 1970 watchface.

### ⚡ Timing Features
- **Centiseconds:** `MM:SS.cc`, redrawn every 50ms.
- **Hours:** switches to `HH:MM :SS` past 60 minutes.
- **Laps:** `UP` records a split, up to 20, and holds it on screen for 3 seconds while timing continues.
- **Lap review:** when stopped, `UP` and `DOWN` step through the splits, with the fastest marked `[BEST]`.
- **Micro-LED bar:** a ten-dot chaser tracking the passing seconds.
- **Runs in the background:** leaving the app preserves the elapsed time and running state, recorded as timestamps rather than a running timer.
- **Sound and vibration:** optional, with tones on watches that have a speaker.

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

# 3. Pulsar Timer (Countdown & Pomodoro) `v1.1.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar Timer` |
| **Category** | `Tools & Utilities` (or `Daily`) |
| **Type** | `Watchapp` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `1.1.0` |
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
**Pulsar Timer** is a countdown timer drawn in the same LED dot-matrix style as the Pulsar 1970 watchface.

### ⚡ Features
- **16 presets:** from 10 seconds to 90 minutes, including 25 minutes.
- **Custom durations:** hold `SELECT` to set minutes (0-99) and seconds (0-59) directly.
- **Micro-LED bar:** ten dots showing the proportion of time remaining.
- **Expiry alert:** 30 seconds of flashing banners, vibration, and a tone where supported.
- **Background alarms:** uses the Pebble wakeup service, so the timer fires even after you leave the app.
- **Countdown warning:** beeps at 3, 2 and 1 seconds.
- **Adjust while running:** add or remove 15 seconds or a minute mid-countdown.

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
- Seven LED colourways, with an optional italic slant.
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

# 4. Pulsar Alarm (Multi-Schedule Clock) `v1.1.0`

### 📋 App Store Listing Fields

| Rebble Submission Field | Value to Enter |
| :--- | :--- |
| **App Title** | `Pulsar Alarm` |
| **Category** | `Daily` (or `Tools & Utilities`) |
| **Type** | `Watchapp` |
| **Author / Developer** | `Travers McInerney` |
| **Version** | `1.1.0` |
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
**Pulsar Alarm** is a multi-alarm clock drawn in the same LED dot-matrix style as the Pulsar 1970 watchface.

### ⚡ Features
- **Four alarms**, each with its own time and schedule.
- **Repeat options:** daily, weekdays, weekends, or once.
- **Alarm alert:** 30 seconds of flashing banners, vibration, and a tone where supported.
- **Background alarms:** uses the Pebble wakeup service, so alarms fire even after you leave the app.
- **Snooze:** 9 minutes by default, the interval mechanical clock radios used; also 5, 10 or 15.
- **On-watch editing:** set hours, minutes and repeat rule with a blinking cursor.
- **Sound and vibration:** optional, with tones on watches that have a speaker.

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
- Seven LED colourways, with an optional italic slant.
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
