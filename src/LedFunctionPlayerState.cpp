#include "LedFunctionPlayerState.h"
#include "SonosNFCPlayerModule.h"


void LedFunctionPlayerState::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_PLAYER_STATE);
    }
    auto playerState = openknxSonosNFCPlayer.playerState();
    LedState ledState = LedStateStopped;
    auto pulsingInterval = _lastPulsingInterval;
    switch (playerState)
    {
        case PlayerStateCardReaderInitializing:
            ledState = LedState::LedStateInitialize;
            break;
        case PlayerStateCardReaderError:
            ledState = LedState::LedStateCardError;
            break;
        case PlayerStateCardReaderTagReading:
            ledState = LedState::LedStateTagReading;
            break;
        case PlayerStateCommandProcessing:
            ledState = LedState::LedStateCommandProcessing;
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
        case LedState::LedStateCommandProcessing:
            _ledFunctionGroup->color(OpenKNX::Led::Color::White);
            _ledFunctionGroup->flash(100);
            return;
    }
}
