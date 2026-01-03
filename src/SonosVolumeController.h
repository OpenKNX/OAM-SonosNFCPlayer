#pragma once
#include <WiFi.h>
#include <ESP32Encoder.h>

class SonosChannel;

class SonosVolumeController
{
        ESP32Encoder& _encoder;
        SonosChannel* _speaker;
    public:
        SonosVolumeController(ESP32Encoder& encoder, SonosChannel* sonos);
        void loop();
};