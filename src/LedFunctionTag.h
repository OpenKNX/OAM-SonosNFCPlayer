#pragma once
#include "OpenKNX.h"

#define OPENKNX_LEDFUNC_PLY_TAG 403

class LedFunctionTag
{
#ifdef OPENKNX_LEDFUNC_BASE_TIME
    OpenKNX::Led::FunctionGroup* _ledFunctionGroup = nullptr;
#endif
public:
    void loop();
};

