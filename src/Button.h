#pragma once    
#include "OpenKNX.h"

class SonosNFCPlayerModule;

class Button
{
    protected:
        SonosNFCPlayerModule& _module; 
        bool _lastState = false;
        unsigned long _debounceTime = 0;
        uint8_t _pin;
    protected:
        Button(SonosNFCPlayerModule& module, uint8_t pin);
    public:
        void loop();
        virtual void onPressed() = 0;
};