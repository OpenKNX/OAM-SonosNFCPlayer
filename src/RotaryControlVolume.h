#pragma once
#include "RotaryControl.h"
#include "ChannelSelection.h"


class ESP32Encoder;
class SonosNFCPlayerModule;

class RotaryControlVolume : public RotaryControl
{
        ChannelSelection _channelSelection;
        uint8_t _customChannel;
    public:
        RotaryControlVolume(SonosNFCPlayerModule& module, ESP32Encoder& encoder, ChannelSelection channelSelection, uint8_t customChannel);
        void loop() override;
};