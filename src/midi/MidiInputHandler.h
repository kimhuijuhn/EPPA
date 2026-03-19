#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "MidiMessage.h"

class RtMidiIn;

namespace EPPA {

    using MidiCallback = std::function<void(const MidiMessage&)>;

    // Wraps RtMidiIn to listen for a specific CC number on an optional channel.
    // Fires MidiCallback only for messages that match the configured CC and channel.
    class MidiInputHandler {
    public:
        // cc         : MIDI CC number to listen for (e.g. 11 for expression pedal)
        // channel    : MIDI channel to listen on; -1 accepts all channels
        explicit MidiInputHandler(int cc, int channel = -1);
        ~MidiInputHandler();

        // Returns the names of all available MIDI input ports.
        std::vector<std::string> GetPortNames() const;

        // Opens a MIDI port by name substring match.
        // If portName is empty, opens the first available port.
        // Returns true on success.
        bool OpenPort(const std::string& portName = "");

        // Sets the callback to invoke when a matching CC message is received.
        void SetCallback(MidiCallback callback);

        // Closes the open MIDI port (safe to call if not open).
        void Close();

        bool IsOpen() const;

    private:
        static void RtMidiCallback(
            double timestamp,
            std::vector<unsigned char>* message,
            void* userData);

        std::unique_ptr<RtMidiIn> mMidiIn;
        MidiCallback               mCallback;
        int                        mTargetCC;
        int                        mTargetChannel;
        bool                       mIsOpen = false;
    };

} // namespace EPPA
