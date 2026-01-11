#pragma once
#include "RotaryControl.h"
#include "ChannelSelection.h"

class ESP32Encoder;
class SonosNFCPlayerModule;


class RotaryControlGroupVolume : public RotaryControl
{
        ChannelSelection _channelSelection;
        uint8_t _customChannel;
    public:
        RotaryControlGroupVolume(SonosNFCPlayerModule& playerModule, ESP32Encoder& encoder, ChannelSelection channelSelection, uint8_t customChannel);
        void loop() override;
};