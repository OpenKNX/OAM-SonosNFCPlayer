#pragma once
#include "OpenKNX.h"

#define OPENKNX_LEDFUNC_PLY_PLAYER_STATE 402

class LedFunctionPlayerState
{
    enum LedState
    {
        LedStateNotInitialized = 0,
        LedStateStopped = 1,
        LedStateInitialize = 2,
        LedStateTagReading = 3,
        LedStateCardError = 4,
        LedStatePlayingTag = 5
    };
    LedState _ledState = LedState::LedStateNotInitialized;
    uint16_t _lastPulsingInterval = 0;
#ifdef OPENKNX_LEDFUNC_BASE_TIME
    OpenKNX::Led::FunctionGroup *_ledFunctionGroup = nullptr;
#endif
public:
    void loop();
};
