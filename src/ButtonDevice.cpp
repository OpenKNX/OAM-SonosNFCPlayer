#include "ButtonDevice.h"

ButtonDevice::ButtonDevice(SonosNFCPlayerModule& module, uint8_t pin, int deviceNumber)
    : Button(module, pin), _channelIndex(deviceNumber - 1)
{
}

void ButtonDevice::onPressed()
{
    bool currentValue = KoDEV_CHSwitchFeedback.value(DPT_Switch);
    bool newValue = !currentValue;
    KoDEV_CHSwitchFeedback.valueNoSend(newValue, DPT_Switch);
    KoDEV_CHSwitch.value(newValue, DPT_Switch);
}