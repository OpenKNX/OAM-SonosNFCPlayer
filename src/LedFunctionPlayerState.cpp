#include "LedFunctionPlayerState.h"


void LedFunctionPlayerState::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_PLAYER_STATE);
    }
    _ledFunctionGroup->color(OpenKNX::Led::Color::Green);
    _ledFunctionGroup->on();
}
