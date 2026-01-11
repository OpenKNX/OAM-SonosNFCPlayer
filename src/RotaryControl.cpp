#include "RotaryControl.h"
#include "ESP32Encoder.h"
#include "SonosNFCPlayerModule.h"

RotaryControl::RotaryControl(SonosNFCPlayerModule& module, ESP32Encoder &encoder)
    : _module(module), _encoder(encoder)
{
    _encoder.setCount(0);
}

void RotaryControl::loop()
{
    // Default implementation does nothing
}

void RotaryControl::processInputKo(GroupObject &ko)
{
    // Default implementation does nothing
}