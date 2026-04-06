#pragma once

// Represents which zone of the pedal's travel range the current CC value maps to.
// Dead bands provide hysteresis around zone boundaries to prevent rapid toggling.
enum class PedalZone {
    UNKNOWN,    // Initial state before any MIDI message is received
    OFF,        // Pedal fully released
    DEAD_LOW,   // Hysteresis band below SOLO zone — transparent in state machine
    SOLO,       // Mid-range position
    DEAD_HIGH,  // Hysteresis band above SOLO zone — transparent in state machine
    ON,         // Pedal fully pressed
};
