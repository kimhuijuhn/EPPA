#include "MidiInputHandler.h"

#include <RtMidi.h>
#include <iostream>

namespace EPPA {

MidiInputHandler::MidiInputHandler(int cc, int channel)
    : mTargetCC(cc)
    , mTargetChannel(channel)
{
    mMidiIn = std::make_unique<RtMidiIn>();
}

MidiInputHandler::~MidiInputHandler()
{
    Close();
}

std::vector<std::string> MidiInputHandler::GetPortNames() const
{
    std::vector<std::string> names;
    const unsigned int portCount = mMidiIn->getPortCount();
    names.reserve(portCount);
    for (unsigned int i = 0; i < portCount; ++i) {
        names.push_back(mMidiIn->getPortName(i));
    }
    return names;
}

bool MidiInputHandler::OpenPort(const std::string& portName)
{
    const unsigned int portCount = mMidiIn->getPortCount();
    if (portCount == 0) {
        std::cerr << "[EPPA] No MIDI input ports found.\n";
        return false;
    }

    // Default to port 0 when no name is specified.
    unsigned int targetPort = 0;
    bool found = portName.empty();

    if (!portName.empty()) {
        for (unsigned int i = 0; i < portCount; ++i) {
            if (mMidiIn->getPortName(i).find(portName) != std::string::npos) {
                targetPort = i;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        std::cerr << "[EPPA] MIDI port not found: " << portName << "\n";
        return false;
    }

    // Ignore sysex, MIDI timing clock, and active sensing — only care about CC.
    mMidiIn->ignoreTypes(true, true, true);
    mMidiIn->setCallback(&MidiInputHandler::RtMidiCallback, this);
    mMidiIn->openPort(targetPort, "EPPA Input");
    mIsOpen = true;

    std::cout << "[EPPA] Opened MIDI port: " << mMidiIn->getPortName(targetPort) << "\n";
    return true;
}

void MidiInputHandler::SetCallback(MidiCallback callback)
{
    mCallback = std::move(callback);
}

void MidiInputHandler::Close()
{
    if (mIsOpen) {
        mMidiIn->closePort();
        mIsOpen = false;
    }
}

bool MidiInputHandler::IsOpen() const
{
    return mIsOpen;
}

// Static RtMidi callback — dispatched from RtMidi's internal thread.
void MidiInputHandler::RtMidiCallback(
    double /*timestamp*/,
    std::vector<unsigned char>* message,
    void* userData)
{
    if (!message || message->size() < 3) return;

    auto* self = static_cast<MidiInputHandler*>(userData);

    const uint8_t status = (*message)[0];
    const uint8_t data1  = (*message)[1];  // CC number
    const uint8_t data2  = (*message)[2];  // CC value

    // Control Change messages have status nibble 0xB (0xB0–0xBF).
    if ((status & 0xF0) != 0xB0) return;

    const uint8_t channel = status & 0x0F;

    // Filter by channel if a specific channel was configured.
    if (self->mTargetChannel >= 0 && channel != static_cast<uint8_t>(self->mTargetChannel)) return;

    // Filter by CC number.
    if (data1 != static_cast<uint8_t>(self->mTargetCC)) return;

    if (self->mCallback) {
        self->mCallback({ channel, data1, data2 });
    }
}

} // namespace EPPA
