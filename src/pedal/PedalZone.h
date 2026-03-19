#pragma once

namespace EPPA {

    // Represents which zone of the pedal's travel range the current position maps to.
    enum class PedalZone {
        OFF,        // Pedal fully released (MIDI 0–2)
        DEAD_LOW,   // Hysteresis band below SOLO zone (MIDI 3–29) — transparent in state machine
        SOLO,       // Mid-range position, solos the selected track (MIDI 30–97)
        DEAD_HIGH,  // Hysteresis band above SOLO zone (MIDI 98–124) — transparent in state machine
        ON,         // Pedal fully pressed, starts playback (MIDI 125–127)
        UNKNOWN     // Initial state before any MIDI message is received
    };

} // namespace EPPA
