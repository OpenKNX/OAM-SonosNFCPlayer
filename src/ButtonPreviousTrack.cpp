#include "ButtonPreviousTrack.h"
#include "SonosNFCPlayerModule.h"


ButtonPreviousTrack::ButtonPreviousTrack(SonosNFCPlayerModule& module, uint8_t pin, ChannelSelection channelSelection, uint8_t customChannel)
    : Button(module, pin), _channelSelection(channelSelection), _customChannel(customChannel)
{

}

void ButtonPreviousTrack::onPressed()
{
    _module.previousTrack(_channelSelection, _customChannel);
}