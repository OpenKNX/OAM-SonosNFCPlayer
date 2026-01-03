#include "SonosVolumeController.h"
#include "SonosChannel.h"

SonosVolumeController::SonosVolumeController(ESP32Encoder& encoder, SonosChannel* speaker)
    : _encoder(encoder), _speaker(speaker)
{
}

void SonosVolumeController::loop()
{
    long diff = -_encoder.getCount();
    if (diff != 0)
    {
      _encoder.setCount(0);
      diff = constrain(diff, -100, 100);
    //   _ledStrip.forceOn(true);
    //   _ledStrip.loop();
      _speaker->setVolumeRelative(diff);    
     // _ledStrip.forceOn(false);
    }
}