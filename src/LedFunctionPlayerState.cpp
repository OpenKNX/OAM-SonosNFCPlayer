#include "LedFunctionPlayerState.h"
#include "SonosNFCPlayerModule.h"

const char* LedFunctionPlayerState::getPlayerStateName(PlayerState state)
{
    switch (state)
    {
        case PlayerState::CardReaderInitializing:
            return "CardReaderInitializing";
        case PlayerState::Idle:
            return "Idle";
        case PlayerState::CardReaderTagAvailable:
            return "CardReaderTagAvailable";
        case PlayerState::CardReaderTagReading:
            return "CardReaderTagReading";
        case PlayerState::PlayingTag:
            return "PlayingTag";
        case PlayerState::CommandProcessing:
            return "CommandProcessing";
        case PlayerState::SettingVolume:
            return "SettingVolume";
        case PlayerState::CardReaderError:
            return "CardReaderError";
        default:
            return "Unknown";
    }
}

void LedFunctionPlayerState::loop()
{
    if (_ledFunctionGroup == nullptr)
    {
        _ledFunctionGroup = openknx.ledFunctions.get(OPENKNX_LEDFUNC_PLY_PLAYER_STATE);
    }
    auto playerState = openknxSonosNFCPlayer.playerState();
    auto pulsingInterval = openknxSonosNFCPlayer.getPulsingInterval();
    auto progMode = knx.progMode();
    if (playerState == _lastPlayerState && pulsingInterval == _lastPulsingInterval && _lastProgMode == progMode)
        return;
    logDebug("StateLED", "Player state changed from %s to %s", getPlayerStateName(_lastPlayerState), getPlayerStateName(playerState));
    _lastPlayerState = playerState;
    _lastPulsingInterval = pulsingInterval;
    _lastProgMode = progMode;
  
    if (progMode)
    {
        _ledFunctionGroup->color(OpenKNX::Led::Color::Red);
        _ledFunctionGroup->on();
        return;
    }
    switch (playerState)
    {
        case PlayerState::CardReaderInitializing:
            _ledFunctionGroup->off();
            return;
        case PlayerState::Idle:
        case PlayerState::CardReaderTagAvailable:
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
        case PlayerState::SettingVolume:
            _ledFunctionGroup->color(OpenKNX::Led::Color::Magenta);
            _ledFunctionGroup->on();
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
            logError("StateLED", "Unknown player state: %d", (int)playerState);
            return;
    }
}
