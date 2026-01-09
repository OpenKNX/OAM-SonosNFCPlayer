#include "LedFunctionTag.h"
#include "SonosNFCPlayerModule.h"

const char* LedFunctionTag::getLedStateName(LedState state)
{
    switch (state)
    {
        case LedFunctionTag::LedState::NotInitialized:
            return "NotInitialized";
        case LedFunctionTag::LedState::HasNotTag:
            return "HasNotTag";
        case LedFunctionTag::LedState::HasTag:
            return "HasTag";
        default:
            return "Unknown";
    }
}

bool hasTag = false;
void LedFunctionTag::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_TAG);
    }
    LedState ledState = openknxSonosNFCPlayer.hasTag() ? LedState::HasTag : LedState::HasNotTag;
    if (ledState == _ledState)
        return;
    logDebug("TagLED", "Tag LED state changed from %s to %s", getLedStateName(_ledState), getLedStateName(ledState));
    _ledState = ledState;
    switch (_ledState)
    {
        case LedState::HasNotTag:
            _ledFunctionGroup->off();
            return;
        case LedState::HasTag:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Blue);
            _ledFunctionGroup->on();
            return;
        default:
            return;
    }
}
