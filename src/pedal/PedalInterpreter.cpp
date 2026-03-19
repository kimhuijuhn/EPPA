#include "PedalInterpreter.h"

#include "../AppConfig.h"

namespace EPPA {

PedalInterpreter::PedalInterpreter(const AppConfig& config)
    : mOffMax(config.offMax)
    , mSoloLow(config.soloLow)
    , mSoloHigh(config.soloHigh)
    , mOnMin(config.onMin)
{}

PedalZone PedalInterpreter::Interpret(int value) const
{
    if (value <= mOffMax)    return PedalZone::OFF;
    if (value < mSoloLow)   return PedalZone::DEAD_LOW;
    if (value <= mSoloHigh) return PedalZone::SOLO;
    if (value < mOnMin)     return PedalZone::DEAD_HIGH;
    return PedalZone::ON;
}

} // namespace EPPA
