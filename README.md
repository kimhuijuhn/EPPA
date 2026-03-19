# EPPA — Expression Pedal Playback Assistant

EPPA is a C++ accessibility tool that maps an expression pedal's MIDI output to Pro Tools transport and solo controls. It is designed for visually impaired users who need reliable, foot-based session navigation without depending on visual feedback.

---

## How It Works

EPPA reads MIDI CC messages from your expression pedal in real time and maps the pedal's position to one of three actions in Pro Tools:

| Pedal Position | MIDI Value | Action |
|---|---|---|
| Full ON (pressed) | 125 – 127 | Start playback |
| Mid-range | 30 – 97 | Solo the currently selected track |
| Full OFF (released) | 0 – 2 | Stop playback |

Values between zones (3–29 and 98–124) are dead bands that prevent accidental toggling when the pedal hovers near a boundary.

When the pedal leaves the solo zone in either direction, the track is automatically unsoloed before the next action fires.

---

## Requirements

- **Pro Tools** 2024.10 or later (running and accessible on `localhost:31416`)
- **PTSL C++ SDK** 2025.10.0 or later
- **CMake** 3.25+
- **Conan** 2.0+
- **C++17** compiler (Xcode on macOS, MSVC on Windows)

---

## Build Instructions

### 1. Install dependencies via Conan

```bash
cd EPPA
mkdir build && cd build
conan install ../Config --output-folder=. --build=missing
```

### 2. Configure with CMake

Point `PTSL_SDK_DIR` at the root of your extracted PTSL SDK:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DPTSL_SDK_DIR=/path/to/PTSL_SDK_CPP.2025.10.0.1267955
```

### 3. Build

```bash
cmake --build .
```

The `eppa` binary will be in your build directory.

---

## Configuration

All settings live in `src/AppConfig.h`. The defaults work for a standard expression pedal with no changes, but you can adjust them before building:

```cpp
int midiCC       = 11;   // CC number your pedal sends (11 = standard expression)
int midiChannel  = -1;   // -1 accepts any channel; 0–15 for a specific channel
std::string midiPortName = "";  // empty = open the first available port
                                // set to a substring of your device name to target it
```

### Zone thresholds

The default ±2 hysteresis buffer around the 25% and 75% boundaries:

```cpp
int offMax   = 2;    // OFF zone:  0–2
int soloLow  = 30;   // SOLO zone: 30–97
int soloHigh = 97;
int onMin    = 125;  // ON zone:   125–127
```

Increase the buffer values (e.g. `soloLow = 28`, `soloHigh = 99`) if your pedal is noisy at the boundaries.

---

## Running EPPA

1. Open Pro Tools and load a session.
2. Select the track you want to solo using Pro Tools' standard track selection (click the track name).
3. Run EPPA from a terminal:

```bash
./build/eppa
```

EPPA will print the available MIDI ports, connect to Pro Tools, and begin listening. Press **Ctrl+C** to quit cleanly.

### Example output

```
[EPPA] Available MIDI ports:
  - Roland EV-5 Expression Pedal
[EPPA] Opened MIDI port: Roland EV-5 Expression Pedal
[EPPA] Connected to Pro Tools. Session ID: abc123
[EPPA] Listening for pedal input. Press Ctrl+C to quit.
[EPPA] Soloed track: Vocals
[EPPA] Starting playback.
[EPPA] Stopping playback.
[EPPA] Unsoloed track: Vocals
[EPPA] Shutdown complete.
```

---

## Targeting a Specific MIDI Port

If you have multiple MIDI devices connected, set `midiPortName` in `AppConfig.h` to a substring of your pedal's port name:

```cpp
std::string midiPortName = "EV-5";  // matches "Roland EV-5 Expression Pedal"
```

Run EPPA once with an empty name to see all available port names printed at startup.

---

## Troubleshooting

**"No MIDI input ports available"**
Your OS does not see any MIDI devices. Check that your pedal is connected and recognized in your system's MIDI settings (Audio MIDI Setup on macOS, Device Manager on Windows).

**"MIDI port not found: \<name\>"**
The substring in `midiPortName` did not match any port. Run EPPA with `midiPortName = ""` to print all available port names, then update the config to match.

**"Failed to connect to Pro Tools"**
- Confirm Pro Tools is running and a session is open.
- Confirm the PTSL API is enabled in Pro Tools preferences.
- Confirm nothing else is blocking `localhost:31416`.

**Pedal triggers actions at the wrong positions**
Adjust `offMax`, `soloLow`, `soloHigh`, or `onMin` in `AppConfig.h`. Use a MIDI monitor tool to observe the raw CC values your pedal sends at each position.

**Solo zone doesn't unsolo when expected**
The track is unsoloed automatically when the pedal moves from the SOLO zone to either ON or OFF. If Pro Tools was already in a solo state before EPPA started, EPPA will not touch it — it only manages solos it created itself.

---

## Project Structure

```
EPPA/
├── src/
│   ├── main.cpp                        Entry point, signal handling
│   ├── AppConfig.h                     All tunable parameters
│   ├── App.h / App.cpp                 Top-level orchestrator
│   ├── midi/
│   │   ├── MidiMessage.h               CC message struct
│   │   ├── MidiInputHandler.h/.cpp     RtMidi wrapper
│   ├── pedal/
│   │   ├── PedalZone.h                 Zone enum
│   │   ├── PedalInterpreter.h/.cpp     MIDI value → zone mapping
│   │   └── PedalStateMachine.h/.cpp    Zone transition logic
│   └── controller/
│       └── ProToolsController.h/.cpp   PTSL SDK wrapper
├── Config/
│   └── conanfile.txt                   RtMidi + nlohmann_json deps
├── CMakeLists.txt
├── references.md                       Full method-level API reference
└── CLAUDE.md                           Claude Code workflow rules
```

---

## Method Reference

See [`references.md`](references.md) for a complete reference of every class and method: parameters, return values, and internal processing logic.

---

## License

EPPA depends on the Avid PTSL C++ SDK, which is subject to the Avid SDK license agreement. Refer to the SDK's `licenses/` directory for third-party license information.
