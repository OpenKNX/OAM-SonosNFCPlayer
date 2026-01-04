#include "LedFunctionTag.h"
#include "SonosNFCPlayerModule.h"

bool hasTag = false;
void LedFunctionTag::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_TAG);
    }
    LedState ledState = openknxSonosNFCPlayer.hasTag() ? LedState::LedStateHasTag : LedState::LedStateHasNotTag;
#ifdef DEBUG_CARD_REMOVE
    if (ledState == LedState::LedStateHasNotTag && _ledState == LedState::LedStateHasTag)
    {
        _lastStoppedTime = millis();
    }
    else if (_lastStoppedTime != 0)
    {
        if (millis() - _lastStoppedTime > 5000)
        {
            _lastStoppedTime = 0;
        }
        else
        {
           ledState  = LedState::LedStateStopped;
        }
    }
 #endif
   
    if (ledState == _ledState)
        return;
    _ledState = ledState;
    logError("LED", "DEBUG: LED Function Tag changed to %d", (int)_ledState);
    switch (_ledState)
    {
        case LedState::LedStateHasNotTag:
            _ledFunctionGroup->off();
            return;
        case LedState::LedStateHasTag:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Blue);
            _ledFunctionGroup->on();
            return;
        case LedState::LedStateStopped:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Red);
            _ledFunctionGroup->blinking(100);
            return;
        default:
            return;
    }
}
