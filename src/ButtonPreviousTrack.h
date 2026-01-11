#pragma once
#include "Button.h"
#include "ChannelSelection.h"

class ButtonPreviousTrack : public Button
{
    ChannelSelection _channelSelection;
    uint8_t _customChannel;
    public :
        ButtonPreviousTrack(SonosNFCPlayerModule& module, uint8_t pin, ChannelSelection channelSelection, uint8_t customChannel = 0);
        void onPressed() override;
};
