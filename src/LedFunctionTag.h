#pragma once
#include "OpenKNX.h"

#define OPENKNX_LEDFUNC_PLY_TAG 403

class LedFunctionTag
{
    enum class LedState
    {
        NotInitialized = 0,
        HasNotTag = 1,
        HasTag = 2,

    };
    LedState _ledState = LedState::NotInitialized;
    OpenKNX::Led::FunctionGroup* _ledFunctionGroup = nullptr;
    const char* getLedStateName(LedState state);
public:
    void loop();
};

