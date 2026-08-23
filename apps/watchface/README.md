# Pulsar 1970 Watchface

<p align="center">
  <img src="../../screenshots/mockups/pebble-time2-device.png" width="40%" alt="Pebble Time 2 Watchface Mockup" />
</p>

A high-fidelity retro digital watchface inspired by the iconic **1970s Hamilton Pulsar** ("Time Computer" P1, P2, and P3 models)—the world's first commercial digital LED wristwatch.

Built for the **Rebble / RePebble** ecosystem with native support for the **Pebble Time 2 (`emery`)**, **Pebble Time / Time Steel (`basalt`)**, **Pebble 2 (`diorite`)**, and classic **Pebble (`aplite`)**.

## Features

- **Procedural 5×7 GaAsP Dot-Matrix LEDs:** Scalable circular LED dies rendered in C with sub-surface ghost dies.
- **Authentic 12° Italic Slant:** True 1970s LED semiconductor die angle.
- **6 Display Modes (Flick & Tap):** Time, Live Seconds (`:SS`), Calendar Date, Steps with Overdrive celebrations, Battery, and Live Heart Rate (BPM).
- **7 Retro Colorways:** Vibrant Ruby Red, Hot Lava, GaP Green, Amber Gold, Cobalt Blue, Lunar White, and Inverted Paper.
- **Clock-Synchronized Charging Animations:** Cylon / Knight Rider ping-pong chaser, cascading flow, breathing pulse, and theater marquee.
- **Bedside Nightlight Mode:** Keeps backlight illuminated while on charging stand.
- **Audio & Haptic Chimes:** 8-bit retro synth speaker chimes and vibration pulses for hourly marks, goal celebrations, and Bluetooth alerts.

## Building

```bash
devenv shell -- pebble build
```
