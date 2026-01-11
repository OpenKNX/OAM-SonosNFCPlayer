#pragma once
#include "OpenKNX.h"

class ESP32Encoder;
class SonosNFCPlayerModule;
class GroupObject;

class RotaryControl 
{
protected:
    SonosNFCPlayerModule& _module;
    ESP32Encoder &_encoder;
public:
    RotaryControl(SonosNFCPlayerModule& module, ESP32Encoder &encoder);
    virtual void loop();
    virtual void processInputKo(GroupObject &ko);
};