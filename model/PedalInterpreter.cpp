#include "PedalInterpreter.h"

PedalInterpreter::PedalInterpreter(PedalThresholds thresholds)
    : mThresholds(thresholds)
{
}

PedalZone PedalInterpreter::Interpret(int value) const
{
    if (value <= mThresholds.offMax)   return PedalZone::OFF;
    if (value < mThresholds.soloLow)   return PedalZone::DEAD_LOW;
    if (value <= mThresholds.soloHigh) return PedalZone::SOLO;
    if (value < mThresholds.onMin)     return PedalZone::DEAD_HIGH;
    return PedalZone::ON;
}
