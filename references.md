# EPPA Method Reference

Expression Pedal Playback Assistant — complete reference for all methods, their parameters, return values, and internal processing.

---

## `AppConfig` (`src/AppConfig.h`)

Plain data struct — no methods. All fields have defaults usable out of the box.

| Field | Default | Description |
|---|---|---|
| `midiCC` | `11` | MIDI CC number to listen for (CC11 = expression pedal) |
| `midiChannel` | `-1` | MIDI channel filter; `-1` accepts all channels |
| `midiPortName` | `""` | Port name substring to match; empty opens the first port |
| `offMax` | `2` | Upper inclusive bound of the OFF zone (0–2) |
| `soloLow` | `30` | Lower inclusive bound of the SOLO zone (30–97) |
| `soloHigh` | `97` | Upper inclusive bound of the SOLO zone (30–97) |
| `onMin` | `125` | Lower inclusive bound of the ON zone (125–127) |
| `ptslAddress` | `"localhost:31416"` | gRPC address of the PTSL server |
| `companyName` | `"EPPA"` | Company name sent to Pro Tools during RegisterConnection |
| `appName` | `"Expression Pedal Playback Assistant"` | App name sent during RegisterConnection |

---

## `MidiMessage` (`src/midi/MidiMessage.h`)

Plain data struct representing a single MIDI Control Change event.

| Field | Type | Description |
|---|---|---|
| `channel` | `uint8_t` | MIDI channel (0–15) |
| `cc` | `uint8_t` | CC number |
| `value` | `uint8_t` | CC value (0–127) |

---

## `MidiInputHandler` (`src/midi/MidiInputHandler.h/.cpp`)

Wraps RtMidiIn and filters incoming MIDI to a single CC number and optional channel.

---

### `MidiInputHandler(int cc, int channel = -1)`
**Parameters**
- `cc` — MIDI CC number to listen for.
- `channel` — MIDI channel (0–15); `-1` accepts any channel.

**Returns** — (constructor)

**Internal processing** — Constructs an `RtMidiIn` instance. Stores `cc` and `channel` for use in the static callback filter.

---

### `GetPortNames() const → std::vector<std::string>`
**Parameters** — none

**Returns** — List of all available MIDI input port names reported by the OS.

**Internal processing** — Calls `RtMidiIn::getPortCount()` and `getPortName(i)` for each index, building a name vector.

---

### `OpenPort(const std::string& portName = "") → bool`
**Parameters**
- `portName` — Substring to match against available port names. Empty string opens port index 0.

**Returns** — `true` on success, `false` if no matching port is found or no ports exist.

**Internal processing**
1. Checks `portCount > 0`; returns false if no ports.
2. If `portName` is empty, selects index 0.
3. Otherwise iterates ports and picks the first whose name contains `portName`.
4. Calls `ignoreTypes(true, true, true)` to discard sysex, timing, and active sensing.
5. Registers `RtMidiCallback` as the RtMidi callback with `this` as user data.
6. Opens the port with `RtMidiIn::openPort`.

---

### `SetCallback(MidiCallback callback)`
**Parameters**
- `callback` — `std::function<void(const MidiMessage&)>` to invoke on matching CC events.

**Returns** — void

**Internal processing** — Move-assigns the callback into `mCallback`. Called by App before `OpenPort`.

---

### `Close()`
**Parameters** — none

**Returns** — void

**Internal processing** — If `mIsOpen`, calls `RtMidiIn::closePort()` and sets `mIsOpen = false`. Safe to call multiple times.

---

### `IsOpen() const → bool`
**Parameters** — none

**Returns** — `true` if a port is currently open.

---

### `RtMidiCallback(double, std::vector<unsigned char>*, void*)` *(static, private)*
**Parameters** — Standard RtMidi callback signature. `userData` is cast to `MidiInputHandler*`.

**Returns** — void

**Internal processing**
1. Rejects messages shorter than 3 bytes.
2. Checks that the status nibble is `0xB` (Control Change); discards anything else.
3. Extracts `channel` from the low nibble of the status byte.
4. Rejects if `mTargetChannel >= 0` and channel does not match.
5. Rejects if `data1` (CC number) does not match `mTargetCC`.
6. Constructs a `MidiMessage` and invokes `mCallback`.

---

## `PedalZone` (`src/pedal/PedalZone.h`)

Enum class describing which region of the pedal's travel range is active.

| Value | Meaning |
|---|---|
| `OFF` | Pedal fully released (MIDI 0–2) |
| `DEAD_LOW` | Hysteresis band below SOLO (MIDI 3–29); transparent in state machine |
| `SOLO` | Mid-range, solos selected track (MIDI 30–97) |
| `DEAD_HIGH` | Hysteresis band above SOLO (MIDI 98–124); transparent in state machine |
| `ON` | Pedal fully pressed, starts playback (MIDI 125–127) |
| `UNKNOWN` | Initial state before any MIDI input arrives |

---

## `PedalInterpreter` (`src/pedal/PedalInterpreter.h/.cpp`)

Pure mapping layer — converts raw MIDI values to `PedalZone`. No state, no side effects.

---

### `PedalInterpreter(const AppConfig& config)`
**Parameters**
- `config` — Reads `offMax`, `soloLow`, `soloHigh`, `onMin` from the config.

**Returns** — (constructor)

**Internal processing** — Copies the four threshold values from `config` into member variables.

---

### `Interpret(int value) const → PedalZone`
**Parameters**
- `value` — Raw MIDI CC value (0–127).

**Returns** — The `PedalZone` that the value maps to.

**Internal processing** — Sequential threshold comparisons in ascending order:
1. `value <= offMax` → `OFF`
2. `value < soloLow` → `DEAD_LOW`
3. `value <= soloHigh` → `SOLO`
4. `value < onMin` → `DEAD_HIGH`
5. Otherwise → `ON`

---

## `PedalStateMachine` (`src/pedal/PedalStateMachine.h/.cpp`)

Tracks the active zone and fires a `ZoneChangeCallback` only on real zone transitions. Dead bands are transparent: they are discarded without updating the active zone.

---

### `SetOnZoneChange(ZoneChangeCallback callback)`
**Parameters**
- `callback` — `std::function<void(PedalZone previous, PedalZone current)>` called on every active-zone transition.

**Returns** — void

**Internal processing** — Move-assigns the callback into `mOnZoneChange`.

---

### `Process(PedalZone newZone)`
**Parameters**
- `newZone` — The zone returned by `PedalInterpreter::Interpret` for the latest CC value.

**Returns** — void

**Internal processing**
1. If `newZone` is `DEAD_LOW` or `DEAD_HIGH`, returns immediately (no state change).
2. If `newZone == mCurrentZone`, returns immediately (no transition).
3. Saves `mCurrentZone` as `previous`, updates `mCurrentZone = newZone`.
4. Fires `mOnZoneChange(previous, newZone)`.

---

### `GetCurrentZone() const → PedalZone`
**Parameters** — none

**Returns** — The currently active zone (`UNKNOWN` if `Process` has never produced a real zone change).

---

## `ProToolsController` (`src/controller/ProToolsController.h/.cpp`)

Wraps the PTSL SDK `CppPTSLClient`. All public methods follow the pattern: query live Pro Tools state → act only if needed. This ensures correctness for VI users who cannot visually verify state.

Uses the JSON-based `SendRequest` API introduced in Pro Tools 2024.10.

---

### `ProToolsController(const AppConfig& config)`
**Parameters**
- `config` — Reads `ptslAddress`, and passes `Mode::Mode_ProTools` and `SkipHostLaunch::SHLaunch_Yes` to `CppPTSLClient`.

**Returns** — (constructor)

**Internal processing** — Constructs a `ClientConfig` and instantiates `CppPTSLClient`. Does not connect yet — `Connect()` must be called separately.

---

### `Connect() → bool`
**Parameters** — none

**Returns** — `true` if Pro Tools accepted the `RegisterConnection` command.

**Internal processing**
1. Builds JSON body `{"company_name": "EPPA", "application_name": "..."}`.
2. Creates `CppPTSLRequest(CId_RegisterConnection, body)` and calls `SendRequest().get()`.
3. Checks `response.GetStatus() == TStatus_Completed`.
4. On success, sets `mIsConnected = true`. The SDK automatically stores the session ID internally.

---

### `IsConnected() const → bool`
**Parameters** — none

**Returns** — `true` after a successful `Connect()` call.

---

### `Play() → bool`
**Parameters** — none

**Returns** — `true` if playback is running after this call (either it was already playing or the toggle succeeded).

**Internal processing**
1. Calls `IsPlaying()`.
2. If not playing, calls `TogglePlay()`.
3. If already playing, returns `true` immediately (no-op).

---

### `Stop() → bool`
**Parameters** — none

**Returns** — `true` if playback is stopped after this call.

**Internal processing**
1. Calls `IsPlaying()`.
2. If playing, calls `TogglePlay()`.
3. If already stopped, returns `true` immediately (no-op).

---

### `SoloSelectedTrack() → bool`
**Parameters** — none

**Returns** — `true` if the selected track was successfully soloed.

**Internal processing**
1. If `mSoloedTrack` is non-empty, calls `SetTrackSolo(mSoloedTrack, false)` and clears `mSoloedTrack`. This ensures only one track is ever soloed by EPPA at a time.
2. Calls `GetSelectedTrackName()` to retrieve the currently selected track.
3. If a name is returned, calls `SetTrackSolo(name, true)`.
4. On success, stores the track name in `mSoloedTrack`.

---

### `UnsoloCurrentTrack() → bool`
**Parameters** — none

**Returns** — `true` if the stored track was successfully unsoloed, or `true` immediately if no track is stored.

**Internal processing**
1. If `mSoloedTrack` is empty, returns `true` (nothing to do).
2. Calls `SetTrackSolo(mSoloedTrack, false)`.
3. On success, clears `mSoloedTrack`.

---

### `IsPlaying() → bool` *(private)*
**Parameters** — none

**Returns** — `true` if Pro Tools reports `TState_TransportPlaying`.

**Internal processing**
1. Creates `CppPTSLRequest(CId_GetTransportState)` with no body.
2. Calls `SendRequest().get()` and checks status.
3. Parses `response.GetResponseBodyJson()` as JSON.
4. Reads `current_setting` field; returns true for `"TState_TransportPlaying"` or its deprecated alias `"TS_TransportPlaying"`.

---

### `TogglePlay() → bool` *(private)*
**Parameters** — none

**Returns** — `true` if `TogglePlayState` completed successfully.

**Internal processing** — Creates `CppPTSLRequest(CId_TogglePlayState)` with no body, calls `SendRequest().get()`, checks `TStatus_Completed`.

---

### `GetSelectedTrackName() → std::string` *(private)*
**Parameters** — none

**Returns** — The name of the first selected track, or empty string if none is selected or the command fails.

**Internal processing**
1. Builds JSON body: `{"track_filter_list": [{"filter": "TLFilter_Selected", "is_inverted": false}], "pagination_request": {"limit": 1, "offset": 0}}`.
2. Creates `CppPTSLRequest(CId_GetTrackList, body)` and calls `SendRequest().get()`.
3. Parses the response body JSON and reads `track_list[0].name`.

---

### `SetTrackSolo(const std::string& trackName, bool enabled) → bool` *(private)*
**Parameters**
- `trackName` — Name of the track to solo or unsolo. Returns `false` immediately if empty.
- `enabled` — `true` to solo, `false` to unsolo.

**Returns** — `true` if `SetTrackSoloState` completed successfully.

**Internal processing**
1. Builds JSON body: `{"track_names": ["<trackName>"], "enabled": <bool>}`.
2. Creates `CppPTSLRequest(CId_SetTrackSoloState, body)` and calls `SendRequest().get()`.
3. Returns `true` if `TStatus_Completed`.

---

## `App` (`src/App.h/.cpp`)

Top-level orchestrator. Owns all components and wires them together.

---

### `App(AppConfig config)`
**Parameters**
- `config` — Configuration passed down to each component.

**Returns** — (constructor)

**Internal processing** — Move-stores config, constructs `MidiInputHandler`, `PedalInterpreter`, and `ProToolsController` from config. `PedalStateMachine` is default-constructed.

---

### `Start() → bool`
**Parameters** — none

**Returns** — `true` if all components initialized successfully.

**Internal processing**
1. Calls `mController.Connect()`; returns false on failure.
2. Registers `OnZoneChange` lambda with `mStateMachine.SetOnZoneChange`.
3. Registers a MIDI callback lambda with `mMidi.SetCallback` that calls `mInterpreter.Interpret(value)` then `mStateMachine.Process(zone)`.
4. Logs available MIDI ports.
5. Calls `mMidi.OpenPort(mConfig.midiPortName)`.
6. Sets `mRunning = true`.

---

### `Run()`
**Parameters** — none

**Returns** — void. Blocks until `mRunning` becomes false.

**Internal processing** — Spins on a 100ms sleep loop. MIDI callbacks fire on RtMidi's internal thread; this loop simply keeps the process alive.

---

### `Shutdown()`
**Parameters** — none

**Returns** — void

**Internal processing**
1. Guards against double-call with `if (!mRunning) return`.
2. Sets `mRunning = false` to unblock `Run()`.
3. Calls `mMidi.Close()`.
4. Calls `mController.UnsoloCurrentTrack()` to leave Pro Tools in a clean state.

---

### `OnZoneChange(PedalZone previous, PedalZone current)` *(private)*
**Parameters**
- `previous` — The zone that was active before this transition.
- `current` — The zone now active.

**Returns** — void

**Internal processing** — Dispatches Pro Tools actions based on `current`:

| `current` | `previous` | Actions taken |
|---|---|---|
| `ON` | `SOLO` | `UnsoloCurrentTrack()` → `Play()` |
| `ON` | anything else | `Play()` |
| `SOLO` | any | `SoloSelectedTrack()` |
| `OFF` | `SOLO` | `UnsoloCurrentTrack()` → `Stop()` |
| `OFF` | anything else | `Stop()` |
| `UNKNOWN` / dead band | — | *(never reached; state machine filters these)* |

Note: When transitioning from `ON` to `SOLO` or `OFF` to `SOLO`, `SoloSelectedTrack()` internally handles unsoloing any previously stored track, so no explicit unsolo is needed before entering the SOLO zone from a non-solo direction.
