#include "RotaryControlGroupVolume.h"
#include "ESP32Encoder.h"
#include "SonosNFCPlayerModule.h"

RotaryControlGroupVolume::RotaryControlGroupVolume(SonosNFCPlayerModule& playerModule, ESP32Encoder &encoder, ChannelSelection channelSelection, uint8_t customChannel)
    : RotaryControl(playerModule, encoder), _channelSelection(channelSelection), _customChannel(customChannel)
{
    _encoder.setCount(0);
}

void RotaryControlGroupVolume::loop()
{
    RotaryControl::loop();
    long diff = -_encoder.getCount();
    if (diff != 0)
    {
        _encoder.setCount(0);
        diff = constrain(diff, -100, 100);
        
        _module.setGroupVolumeRelative(_channelSelection, _customChannel, diff);
    }
}