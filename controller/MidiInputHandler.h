#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "RtMidi.h"

// Callback type: invoked when a matching CC message arrives.
// Parameters: cc number, value (0–127)
using MidiCCCallback = std::function<void(int cc, int value)>;

// Wraps RtMidiIn to listen for CC messages on a specific channel.
// Fires MidiCCCallback only for messages matching the configured CC and channel.
class MidiInputHandler {
public:
    // cc      : MIDI CC number to listen for (e.g. 11 for expression pedal)
    // channel : MIDI channel to filter on; -1 accepts all channels
    explicit MidiInputHandler(int cc, int channel = -1);
    ~MidiInputHandler();

    // Returns the names of all available MIDI input ports.
    std::vector<std::string> GetPortNames() const;

    // Opens a port by name substring match.
    // If portName is empty, opens the first available port.
    bool OpenPort(const std::string& portName = "");

    // Registers the callback to invoke on each matching CC message.
    void SetCallback(MidiCCCallback callback);

    void Close();
    bool IsOpen() const;

private:
    static void RtMidiCallback(double timestamp,
                               std::vector<unsigned char>* message,
                               void* userData);

    std::unique_ptr<RtMidiIn> mMidiIn;
    MidiCCCallback             mCallback;
    int                        mTargetCC;
    int                        mTargetChannel;
    bool                       mIsOpen = false;
};
