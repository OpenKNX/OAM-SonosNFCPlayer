#include "LedFunctionPlayerState.h"
#include "SonosNFCPlayerModule.h"


void LedFunctionPlayerState::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_PLAYER_STATE);
    }
    auto playerState = openknxSonosNFCPlayer.playerState();
    auto pulsingInterval = openknxSonosNFCPlayer.getPulsingInterval();
    if (playerState == _lastPlayerState && pulsingInterval == _lastPulsingInterval)
        return;
    _lastPlayerState = playerState;
    _lastPulsingInterval = pulsingInterval;
  
    switch (playerState)
    {
        case PlayerState::Idle:
        case PlayerState::CardReaderAvailable:
            _ledFunctionGroup->off();
            return;
        case PlayerState::CardReaderError:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Red);
            _ledFunctionGroup->blinking(250);
            return;
        case PlayerState::CardReaderTagReading:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Green);
            _ledFunctionGroup->on();
            return;
        case PlayerState::CardReaderInitializing:
            _ledFunctionGroup->off();
            return;
        case PlayerState::PlayingTag:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Blue);
            _ledFunctionGroup->pulsing(pulsingInterval);
            return;
        case PlayerState::CommandProcessing:
            _ledFunctionGroup->color(OpenKNX::Led::Color::White);
            _ledFunctionGroup->flash(100);
            return;
        default:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Green);
            return;
    }
}
