#pragma once

#include <cstdint>

namespace EPPA {

    // Represents a parsed MIDI Control Change message.
    struct MidiMessage {
        uint8_t channel;  // MIDI channel (0–15)
        uint8_t cc;       // CC number (e.g. 11 for expression)
        uint8_t value;    // CC value (0–127)
    };

} // namespace EPPA
