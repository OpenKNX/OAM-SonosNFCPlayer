#include "LedFunctionTag.h"
#include "SonosNFCPlayerModule.h"

bool hasTag = false;
void LedFunctionTag::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_TAG);
    }
    _ledFunctionGroup->color(OpenKNX::Led::Color::Blue);
    openknxSonosNFCPlayer.hasTag() ?
        _ledFunctionGroup->on():
        _ledFunctionGroup->off();
}
