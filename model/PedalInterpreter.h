#pragma once

#include "PedalZone.h"

struct PedalThresholds {
    int offMax   = 15;
    int soloLow  = 30;
    int soloHigh = 97;
    int onMin    = 110;
};

// Maps raw MIDI CC values (0–127) to PedalZone values.
// Thresholds are configurable; defaults match a standard expression pedal layout:
//   OFF      :   0 –  15
//   DEAD_LOW :  16 –  29  (hysteresis buffer below SOLO)
//   SOLO     :  30 –  97
//   DEAD_HIGH:  98 – 109  (hysteresis buffer above SOLO)
//   ON       : 110 – 127
class PedalInterpreter {
public:
    explicit PedalInterpreter(PedalThresholds thresholds = {});

    // Pure mapping — no side effects, no state.
    PedalZone Interpret(int value) const;

private:
    PedalThresholds mThresholds;
};
