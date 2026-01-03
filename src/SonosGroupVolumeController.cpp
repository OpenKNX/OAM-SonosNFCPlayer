#include "SonosGroupVolumeController.h"
#include "SonosChannel.h"

SonosGroupVolumeController::SonosGroupVolumeController(ESP32Encoder &encoder, SonosChannel *speaker)
    : _encoder(encoder), _speaker(speaker)
{

}

void SonosGroupVolumeController::loop()
{
    long diff = -_encoder.getCount();
    if (diff != 0)
    {
        _encoder.setCount(0);
        diff = constrain(diff, -100, 100);
        // _ledStrip.forceOn(true);
        // _ledStrip.loop();
        
        _speaker->setGroupVolumeRelative(diff);
     //   _ledStrip.forceOn(false);
    }
}