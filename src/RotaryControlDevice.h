#pragma once
#include "RotaryControl.h"


class RotaryControlDevice : public RotaryControl
{
    unsigned long _takeOverLock = 0;
    unsigned long _debounceTime = 0;
    int _channelIndex;
    uint8_t _lastPercentage = 255;
    public:
        RotaryControlDevice(SonosNFCPlayerModule& module, ESP32Encoder& encoder, int deviceNumber);
        void loop() override;
        void processInputKo(GroupObject &ko) override;
};