#include "ButtonPause.h"
#include "SonosNFCPlayerModule.h"


ButtonPause::ButtonPause(SonosNFCPlayerModule& module, uint8_t pin, ChannelSelection channelSelection, uint8_t customChannel)
    : Button(module, pin), _channelSelection(channelSelection), _customChannel(customChannel)
{

}

void ButtonPause::onPressed()
{
    _module.togglePlay(_channelSelection, _customChannel);
}