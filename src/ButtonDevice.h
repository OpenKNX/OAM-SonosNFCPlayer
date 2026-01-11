#pragma once
#include "Button.h"


class ButtonDevice : public Button
{
    int _channelIndex;
    public:
        ButtonDevice(SonosNFCPlayerModule& module, uint8_t pin, int deviceNumber);
        void onPressed() override;
};