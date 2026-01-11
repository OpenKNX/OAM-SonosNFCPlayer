#include "RotaryControlVolume.h"
#include "ESP32Encoder.h"
#include "SonosNFCPlayerModule.h"

RotaryControlVolume::RotaryControlVolume(SonosNFCPlayerModule& module, ESP32Encoder& encoder, ChannelSelection channelSelection, uint8_t customChannel)
    : RotaryControl(module, encoder), _channelSelection(channelSelection), _customChannel(customChannel)
{
}

void RotaryControlVolume::loop()
{
    RotaryControl::loop();
    long diff = -_encoder.getCount();
    if (diff != 0)
    {
      _encoder.setCount(0);
      diff = constrain(diff, -100, 100);
       _module.setVolumeRelative(_channelSelection, _customChannel, diff);    
   }
}