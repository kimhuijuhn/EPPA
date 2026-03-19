#pragma once

#include "PedalZone.h"

namespace EPPA {

    struct AppConfig;

    // Maps raw MIDI CC values (0–127) to PedalZone values.
    // Dead band zones (DEAD_LOW, DEAD_HIGH) represent the hysteresis regions
    // around zone boundaries. The state machine handles them as transparent — it
    // stays in the previous active zone until the pedal clearly enters a new one.
    class PedalInterpreter {
    public:
        explicit PedalInterpreter(const AppConfig& config);

        // Converts a raw MIDI CC value into a PedalZone.
        // Pure function — no side effects, no state.
        PedalZone Interpret(int value) const;

    private:
        int mOffMax;    // Upper bound of OFF zone
        int mSoloLow;   // Lower bound of SOLO zone
        int mSoloHigh;  // Upper bound of SOLO zone
        int mOnMin;     // Lower bound of ON zone
    };

} // namespace EPPA
