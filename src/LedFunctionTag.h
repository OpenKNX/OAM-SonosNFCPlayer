#pragma once
#include "OpenKNX.h"

#define OPENKNX_LEDFUNC_PLY_TAG 403

class LedFunctionTag
{
    unsigned long _lastStoppedTime = 0;
    enum LedState
    {
        LedStateNotInitialized = 0,
        LedStateHasNotTag = 1,
        LedStateHasTag = 2,
        LedStateStopped = 3,

    };
    LedState _ledState = LedState::LedStateNotInitialized;

#ifdef OPENKNX_LEDFUNC_BASE_TIME
    OpenKNX::Led::FunctionGroup* _ledFunctionGroup = nullptr;
#endif
public:
    void loop();
};

