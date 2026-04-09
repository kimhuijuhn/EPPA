#include "MidiInputHandler.h"

#include <iostream>

MidiInputHandler::MidiInputHandler(int cc, int channel)
    : mMidiIn(std::make_unique<RtMidiIn>())
    , mTargetCC(cc)
    , mTargetChannel(channel)
{
}

MidiInputHandler::~MidiInputHandler()
{
    Close();
}

std::vector<std::string> MidiInputHandler::GetPortNames() const
{
    std::vector<std::string> names;
    unsigned int count = mMidiIn->getPortCount();
    for (unsigned int i = 0; i < count; ++i)
        names.push_back(mMidiIn->getPortName(i));
    return names;
}

bool MidiInputHandler::OpenPort(const std::string& portName)
{
    unsigned int count = mMidiIn->getPortCount();
    if (count == 0) {
        std::cerr << "MidiInputHandler: no MIDI input ports available\n";
        return false;
    }

    if (portName.empty()) {
        mMidiIn->openPort(0);
    } else {
        for (unsigned int i = 0; i < count; ++i) {
            if (mMidiIn->getPortName(i).find(portName) != std::string::npos) {
                mMidiIn->openPort(i);
                mIsOpen = true;
                mMidiIn->setCallback(&MidiInputHandler::RtMidiCallback, this);
                mMidiIn->ignoreTypes(false, false, false);
                return true;
            }
        }
        std::cerr << "MidiInputHandler: port \"" << portName << "\" not found\n";
        return false;
    }

    mIsOpen = true;
    mMidiIn->setCallback(&MidiInputHandler::RtMidiCallback, this);
    mMidiIn->ignoreTypes(false, false, false);
    return true;
}

void MidiInputHandler::SetCallback(MidiCCCallback callback)
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

void MidiInputHandler::RtMidiCallback(double /*timestamp*/,
                                      std::vector<unsigned char>* message,
                                      void* userData)
{
    if (!message || message->size() < 3) return;

    auto* self = static_cast<MidiInputHandler*>(userData);

    uint8_t status  = (*message)[0];
    uint8_t cc      = (*message)[1];
    uint8_t value   = (*message)[2];

    // CC messages: status byte 0xB0–0xBF
    if ((status & 0xF0) != 0xB0) return;

    int channel = status & 0x0F;
    std::cout << "[MIDI] ch=" << channel << " CC" << (int)cc << " value=" << (int)value << "\n";

    if (self->mTargetChannel != -1 && channel != self->mTargetChannel) return;
    if (cc != self->mTargetCC) return;

    if (self->mCallback)
        self->mCallback(cc, value);
}
