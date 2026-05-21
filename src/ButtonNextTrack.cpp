#include "ButtonNextTrack.h"
#include "SonosNFCPlayerModule.h"


ButtonNextTrack::ButtonNextTrack(SonosNFCPlayerModule& module, uint8_t pin, ChannelSelection channelSelection, uint8_t customChannel)
    : Button(module, pin), _channelSelection(channelSelection), _customChannel(customChannel)
{

}

void ButtonNextTrack::onPressed()
{
    _module.nextTrack(_channelSelection, _customChannel);
}