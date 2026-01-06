#pragma once
#include "OpenKNX.h"
#include "PlayerState.h"

#define OPENKNX_LEDFUNC_PLY_PLAYER_STATE 402

class LedFunctionPlayerState
{
    PlayerState _lastPlayerState = PlayerState::Idle;
    uint16_t _lastPulsingInterval = 0;
    OpenKNX::Led::FunctionGroup *_ledFunctionGroup = nullptr;
public:
    void loop();
};
