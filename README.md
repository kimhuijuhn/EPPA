# EPPA

> Read this in [Korean (한국어)](README.ko.md).

## Project Summary

EPPA is a hands-free playback control system for **visually impaired musicians and audio engineers**. By mapping a continuous expression pedal to discrete playback states, it lets users drive a DAW (Pro Tools) without having to look at or reach for a screen, mouse, or keyboard.

## Tech Stack

- **C++17** — core application language; chosen for low-latency real-time audio/MIDI handling and direct access to the native PTSL SDK.
- **CMake** — build system; manages the link against the platform framework and SDK headers across configurations.
- **PTSL SDK (Pro Tools Scripting Library)** — drives Pro Tools transport (play / solo / stop) programmatically. This is what turns pedal input into actual DAW actions.
- **RtMidi** — cross-platform MIDI I/O library; receives Continuous Controller (CC) messages from the expression pedal in real time.
- **CoreMIDI / CoreAudio / CoreFoundation (macOS)** — native frameworks RtMidi and PTSL build on; required for low-latency MIDI input on Apple Silicon.
- **macOS `say` / voiceover layer (`Reader`)** — provides spoken feedback so the user can confirm state transitions without visual cues — critical for the accessibility goal.

## Main Features

### Play–Solo–Stop on Pedal Zones
The pedal's continuous travel range is split into discrete zones (`OFF`, `SOLO`, `ON`) with `DEAD_LOW` / `DEAD_HIGH` hysteresis bands between them to suppress jitter at boundaries. Each zone maps to a Pro Tools transport action:

- **OFF** (pedal released) → **Stop**
- **SOLO** (mid-travel) → **Solo** the active track
- **ON** (fully pressed) → **Play**

State transitions are debounced through the dead bands, so a pedal hovering near a boundary does not rapidly toggle playback.

## Troubleshooting

### Pedal Type Compatibility (TS vs. TRS)
We are currently working through a polarity / wiring issue between **TS (mono)** and **TRS (stereo)** expression pedals. Different manufacturers assign the wiper and reference pins to the tip and ring differently, which inverts or flat-lines the CC value the host receives. **This is an ongoing investigation** — we are characterizing the behavior across pedal models and considering a configurable polarity / pin-assignment option in software.

## Outcomes

**WIP** — demo videos in progress.

## Build & Run

```bash
# Build the PTSL SDK once at the repo root, then:
cmake -B build
cmake --build build
./build/EPPAClient
```
