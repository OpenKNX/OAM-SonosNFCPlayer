#include "LedFunctionPlayerState.h"
#include "SonosNFCPlayerModule.h"


void LedFunctionPlayerState::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_PLAYER_STATE);
    }
    auto cardReaderState = openknxSonosNFCPlayer.cardReaderState();
    LedState ledState = LedStateStopped;
    auto pulsingInterval = _lastPulsingInterval;
    switch (cardReaderState)
    {
        case CardReaderState::CARD_READER_STATE_INITIALIZING:
            ledState = LedState::LedStateInitialize;
            break;
        case CardReaderState::CARD_READER_ERROR:
            ledState = LedState::LedStateCardError;
            break;
        case CardReaderState::CARD_READER_STATE_TAG_READING:
            ledState = LedState::LedStateTagReading;
            break;
        default:
            if (openknxSonosNFCPlayer.isPlayingTag())
            {
                ledState = LedState::LedStatePlayingTag;
                pulsingInterval = openknxSonosNFCPlayer.getPulsingInterval();
            }
            else
            {
                ledState = LedState::LedStateStopped;
            }
            break;
    }
    if (ledState == _ledState && pulsingInterval == _lastPulsingInterval)
        return;
    _ledState = ledState;
    _lastPulsingInterval = pulsingInterval;
   
    logError("LED", "DEBUG: LED Function Player State changed to %d", (int)_ledState);

    switch (_ledState)
    {
        case LedState::LedStateInitialize:
            _ledFunctionGroup->off();
            return;
        case LedState::LedStateCardError:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Red);
            _ledFunctionGroup->blinking(250);
            return;
        case LedState::LedStateTagReading:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Green);
            _ledFunctionGroup->on();
            return;
        case LedState::LedStateStopped:
            _ledFunctionGroup->off();
            return;
        case LedState::LedStatePlayingTag:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Blue);
            _ledFunctionGroup->pulsing(pulsingInterval);
            return;
    }
}
