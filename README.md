# Pebble Pulsar Suite ⚡

<p align="center">
  <img src="screenshots/pulsar-time2-hero.png" width="100%" alt="Pebble Pulsar Suite Hero Banner">
</p>

<p align="center">
  <img src="screenshots/mockups/pebble-time2-device.png" width="23%" alt="Pebble Time 2 Watchface" />
  <img src="screenshots/mockups/pebble-basalt-device.png" width="23%" alt="Pebble Time Steel Chrono" />
  <img src="screenshots/mockups/pebble-diorite-device.png" width="23%" alt="Pebble 2 HR Timer" />
  <img src="screenshots/mockups/pebble-aplite-device.png" width="23%" alt="Pebble Classic Alarm" />
</p>

<p align="center">
  <em>The complete 1970s Hamilton Pulsar Solid-State LED Suite for Pebble OS: <strong>Watchface</strong>, <strong>Chronograph</strong>, <strong>Countdown Timer</strong>, and <strong>Multi-Alarm Clock</strong>.</em>
</p>

---

## 🏛️ Monorepo Suite Architecture

The **Pebble Pulsar Suite** is structured as an engineering monorepo containing 4 dedicated standalone applications powered by a shared procedural C graphics and audio engine (`libpulsar`):

```
pebble-pulsar/
├── shared/                         # Core Procedural Engine (libpulsar)
│   ├── include/
│   │   ├── pulsar_palette.h        # 7 Retro LED Colorways & RGBA mappings
│   │   ├── pulsar_matrix.h         # 5x7 GaAsP procedural dot-matrix LED font & 12° slant math
│   │   ├── pulsar_layout.h         # Display geometry (Emery 200x228 vs Basalt/Diorite/Aplite 144x168)
│   │   ├── pulsar_micro_bar.h      # 10-Dot Micro-LED Bar (Tachymeter, Progress depletion, Gauge)
│   │   ├── pulsar_brand.h          # Space-age typography headers & footers
│   │   └── pulsar_audio.h          # 8-bit retro square/sawtooth synth chimes & haptic vibration curves
│   └── src/
│       ├── pulsar_palette.c
│       ├── pulsar_matrix.c
│       ├── pulsar_micro_bar.c
│       ├── pulsar_brand.c
│       └── pulsar_audio.c
├── apps/
│   ├── watchface/                  # Pulsar 1970 Watchface (6 modes, flick-to-wake, nightlight)
│   ├── chrono/                     # Pulsar Chronograph (50ms sub-second precision, 20-lap splits)
│   ├── timer/                      # Pulsar Countdown Timer (Presets, 10-dot depletion, Wakeup API)
│   └── alarm/                      # Pulsar Alarm Clock (4 slots, repeat schedules, 9m snooze)
├── packages/
│   └── pebble-clay/                # Embedded Clay configuration framework
├── scripts/
│   ├── build_all.py                # Automated suite builder compiling all 4 apps for all platforms
│   ├── capture_all_screenshots.py  # Automated emulator test & screenshot capture
│   ├── generate_hardware_mockups.py # 3D composite device rendering
│   └── generate_store_assets.py    # Rebble Appstore banners & icons
├── tests/
│   └── test_firmware.py            # Automated firmware verification & parity test suite
└── dist/                           # Production PBW bundles for Rebble distribution
```

---

## ⚡ Suite Applications

| Application | Type | Primary Features | Pebble OS Integration |
| :--- | :--- | :--- | :--- |
| **[Pulsar Watchface](apps/watchface)** | Watchface | 6 Modes (Time, Seconds, Date, Steps, Battery, Heart Rate), Charging Animations, Nightlight | Accelerometer flick/tap, Pebble Health API, 12-second previews |
| **[Pulsar Chrono](apps/chrono)** | Watchapp | Centisecond stopwatch (`MM:SS.cc`), 20-lap memory buffer, 10-dot optical tachymeter chaser | High-rate 50ms timer loop, Zero-drift persistent epoch math |
| **[Pulsar Timer](apps/timer)** | Watchapp | 10 quick presets (1m–60m + Pomodoro), 10-dot progress depletion, 3-2-1 warning ticks | Pebble `Wakeup API` background wake, escalating ruby alert screen |
| **[Pulsar Alarm](apps/alarm)** | Watchapp | 4 independent alarm slots, repeat rules (Daily, Weekdays, Weekends, Once), 9m vintage snooze | Pebble `Wakeup API` scheduler, escalating haptic & audio wake curves |

---

## 🕹️ Why Separate Watchapps?

1. **Pebble Quick Launch (1-Click Instant Access):** You can assign **Pulsar Chrono** or **Pulsar Timer** directly to a long-press of the **UP** or **DOWN** hardware buttons on your Pebble. Pressing the button instantly launches the app with zero menu navigation.
2. **Unambiguous Hardware Ergonomics:** Clean 3-button mapping per tool without confusing sub-modes or accidental navigation.
3. **Background Wakeup Isolation:** Dedicated `wakeup_schedule` event spaces prevent background timer alarms from conflicting with multi-alarm clock wakeups.
4. **Ultra-Lean Memory Footprint:** On memory-constrained hardware (e.g. Pebble Classic with 24KB RAM), each app uses less than 6KB of RAM, leaving maximum heap available for system tasks.

---

## 🛠️ Developer Workflow & Monorepo Commands

This project uses [`devenv.nix`](devenv.nix) providing the official ARM GCC toolchain and Pebble SDK 4.33.1.

### Build All Suite Applications
```bash
# Compiles all 4 apps across all 4 architectures (Emery, Basalt, Diorite, Aplite) and outputs to dist/
npm run build
# Or directly:
devenv shell -- python3 scripts/build_all.py
```

### Build an Individual App
```bash
npm run build:watchface
npm run build:chrono
npm run build:timer
npm run build:alarm
```

### Run Automated Firmware Verification & Parity Tests
```bash
npm test
# Or directly:
devenv shell -- python3 -m unittest discover tests
```

### Capture Screenshots & Generate Device Mockups
```bash
npm run screenshots
npm run mockups
npm run store-assets
```

---

## 🎨 Design System: The 1970 Hamilton Pulsar

* **Procedural 5×7 GaAsP Dot-Matrix LEDs:** Circular LED semiconductor dies rendered mathematically in C with sub-surface ghost dies (`GColorBulgarianRose`) on pitch black.
* **12° Vintage Italic Slant:** Replicates the physical rightward die tilt characteristic of 1970s Litronix and Bowmar LED modules.
* **7 Colorways:**
  1. *GaAsP Ruby Red (1972)* — Classic high-luminance red.
  2. *Hot Lava Orange* — Warm vintage orange.
  3. *GaP Phosphor Green (1975)* — Early green LED prototype styling.
  4. *Amber Gold (HP-01)* — Hewlett-Packard space-age calculation watch homage.
  5. *Cobalt Blue* — Modern electric blue.
  6. *Lunar White* — High-contrast crisp white.
  7. *Inverted Paper* — Black matrix LEDs on paper-white background for high-glare sunlight.

---

## 📦 Distribution Artifacts

The compiled PBWs in `dist/` are ready for one-click upload to the [Rebble Developer Portal](https://dev-portal.rebble.io/):
* `dist/pebble-pulsar-watchface.pbw`
* `dist/pebble-pulsar-chrono.pbw`
* `dist/pebble-pulsar-timer.pbw`
* `dist/pebble-pulsar-alarm.pbw`

---

## 📜 License

MIT © [Travers McInerney](https://github.com/traversm)
