#include "OpenKNX.h"
#include "RotaryControlDevice.h"
#include "ESP32Encoder.h"

RotaryControlDevice::RotaryControlDevice(SonosNFCPlayerModule& module, ESP32Encoder& encoder, int deviceNumber)
    : RotaryControl(module, encoder), _channelIndex(deviceNumber - 1)
{
    _lastPercentage = KoDEV_CHPercentageFeedback.value(DPT_Scaling);
    _encoder.setCount(_lastPercentage);
}

void RotaryControlDevice::loop()
{
    RotaryControl::loop();
    if (_takeOverLock != 0 && millis() - _takeOverLock > 800)
    {
        _takeOverLock = 0;
        _lastPercentage = KoDEV_CHPercentageFeedback.value(DPT_Scaling);
        _encoder.setCount(_lastPercentage);
    }
    if (_debounceTime != 0)
    {
        if (millis() - _debounceTime > 100)
            _debounceTime = 0;
        else
            return;
    }
    auto encoderValue = -_encoder.getCount();
    if (encoderValue < 0)
    {
        encoderValue = 0;
        _encoder.setCount(0);
    }
    else if (encoderValue > 100)
    {
        encoderValue = 100;
        _encoder.setCount(100);
    }
    uint8_t percentage = (uint8_t) encoderValue;
    if (percentage != _lastPercentage)
    {
        KoDEV_CHPercentage.valueCompare(percentage, DPT_Scaling);
        _lastPercentage = percentage;
        _takeOverLock = max(1UL, millis());
        _debounceTime = _takeOverLock;
    } 
    
}

void RotaryControlDevice::processInputKo(GroupObject &ko)
{
    RotaryControl::processInputKo(ko);
    if (ko.asap() == KoDEV_CHPercentageFeedback.asap() && _takeOverLock == 0)
    {
        uint8_t percentage = (uint8_t) KoDEV_CHPercentageFeedback.value(DPT_Scaling);
        _encoder.setCount(percentage);
    }
}