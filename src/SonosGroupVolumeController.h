#pragma once
#include <WiFi.h>
#include <ESP32Encoder.h>

class SonosChannel;

class SonosGroupVolumeController
{
        ESP32Encoder& _encoder;
        SonosChannel* _speaker;
    public:
        SonosGroupVolumeController(ESP32Encoder& encoder, SonosChannel* speaker);
        void loop();
};