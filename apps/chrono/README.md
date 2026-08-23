# Pulsar Chrono (Stopwatch)

<p align="center">
  <img src="../../screenshots/mockups/pebble-time-steel-device.png" width="40%" alt="Pebble Time Steel Chrono Mockup" />
</p>

A precision digital chronograph and lap timer for Pebble OS styled after the 1970s Hamilton Pulsar LED digital chronometers.

## Features

- **Sub-Second Precision:** 50ms (20fps) high-frequency refresh loop with centisecond readout (`MM:SS.cc`).
- **10-Dot Tachymeter Micro-LED Chaser:** Smooth optical bead animation tracking split-second progress.
- **Lap & Split Recording:** Record up to 20 lap splits with memory recall and review.
- **Zero-Drift Background Persistence:** Exiting or switching apps preserves elapsed time and running state via epoch timestamps with zero background battery drain.
- **Retro Synth Audio & Haptics:** Audio start/stop tones and tactile vibration pulses for button actuation.
- **Clay Configuration:** Colorway selection, 12° Italic Slant toggle, and sound/vibe preferences.

## Controls

- **SELECT (Short Press):** Start / Stop Chronograph.
- **UP (Short Press):**
  - *When Running:* Record Lap Split.
  - *When Stopped:* Browse recorded lap splits.
- **DOWN (Short Press):**
  - *When Stopped:* Reset to `00:00.00` (second press clears recorded lap splits).
- **BACK:** Return to watchface (preserves running stopwatch in background).

## Building

```bash
devenv shell -- pebble build
```
