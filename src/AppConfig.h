#pragma once

#include <string>

namespace EPPA {

    struct AppConfig {
        // --- MIDI settings ---
        int         midiCC          = 11;   // CC11 = standard expression pedal
        int         midiChannel     = -1;   // -1 = accept any channel
        std::string midiPortName    = "";   // empty = open first available port

        // --- Pedal zone thresholds (MIDI value range: 0–127) ---
        //
        //   OFF zone   :  0  .. offMax      (0 – 2)
        //   DEAD_LOW   :  offMax+1 .. soloLow-1    (3 – 29)   → transparent, stays in OFF
        //   SOLO zone  :  soloLow .. soloHigh       (30 – 97)
        //   DEAD_HIGH  :  soloHigh+1 .. onMin-1     (98 – 124) → transparent, stays in SOLO/ON
        //   ON zone    :  onMin .. 127               (125 – 127)
        //
        // The dead bands provide ±2 hysteresis around the 25% (≈32) and 75% (≈95) boundaries.
        int offMax   = 2;
        int soloLow  = 30;   // 25% of 127 = 32, minus 2 buffer
        int soloHigh = 97;   // 75% of 127 = 95, plus 2 buffer
        int onMin    = 125;  // 127, minus 2 buffer

        // --- PTSL connection settings ---
        std::string ptslAddress = "localhost:31416";
        std::string companyName = "EPPA";
        std::string appName     = "Expression Pedal Playback Assistant";
    };

} // namespace EPPA
