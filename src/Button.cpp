#include "Button.h"
#include "SonosNFCPlayerModule.h"

Button::Button(SonosNFCPlayerModule& module, uint8_t pin)
    : _module(module), _pin(pin)
{
    pinMode(_pin, INPUT_PULLUP);
    _lastState = !digitalRead(_pin);
}

void Button::loop()
{
    if (_debounceTime != 0 && millis() - _debounceTime > 100)
    {
        _debounceTime = 0;
    }
    if (_debounceTime != 0)
        return;
    bool currentState = !digitalRead(_pin);
    if (currentState != _lastState)
    {
        _lastState = currentState;
        if (currentState) // Assuming active low button
        {
            onPressed();
            _debounceTime = max(1UL, millis());
        }
    }
}