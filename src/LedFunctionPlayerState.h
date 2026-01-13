#pragma once
#include "OpenKNX.h"
#include "PlayerState.h"

#define OPENKNX_LEDFUNC_PLY_PLAYER_STATE 402

class LedFunctionPlayerState
{
    PlayerState _lastPlayerState = PlayerState::Idle;
    bool _lastProgMode = false;
    uint16_t _lastPulsingInterval = 0;
    OpenKNX::Led::FunctionGroup *_ledFunctionGroup = nullptr;
    const char* getPlayerStateName(PlayerState state);
public:
    void loop();
};
