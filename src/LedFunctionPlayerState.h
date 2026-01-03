#pragma once
#include "OpenKNX.h"

#define OPENKNX_LEDFUNC_PLY_PLAYER_STATE 402

class LedFunctionPlayerState
{
#ifdef OPENKNX_LEDFUNC_BASE_TIME
    OpenKNX::Led::FunctionGroup* _ledFunctionGroup = nullptr;
#endif
public:
    void loop();
};

