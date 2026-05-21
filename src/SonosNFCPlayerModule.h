#if defined(KNX_IP_WIFI) || defined(KNX_IP_LAN)
#pragma once
#include "OpenKNX.h"
#include "CardReader.h"
#include "string"
#include <ESP32Encoder.h>
#include "LedFunctionPlayerState.h"
#include "LedFunctionTag.h"
#include "SonosChannel.h"
#include "PlayerState.h"
#include "ChannelSelection.h"

class SonosChannel;
class RotaryControl;
class Button;


class SonosNFCPlayerModule : public OpenKNX::Module
{
    bool _lastButtonState1 = false;
    bool _lastButtonState2 = false;
    bool _lastButtonState3 = false;
    std::shared_ptr<SonosChannelPlayHandle> _currentPlayHandle = nullptr;
    ESP32Encoder _encoder1;
    ESP32Encoder _encoder2;
    ESP32Encoder _encoder3;
    LedFunctionPlayerState _ledFunctionPlayerState;
    LedFunctionTag _ledFunctionTag;

    bool _startupFinishedPlayAllowed = false;
    
    unsigned  long _lastCommandProcessTime = 0;
    unsigned  long _playingNotPossibleSince = 0;
    bool _handlingCardInProgress = false;
    bool _settingVolumeInProgress = false;
    CardReader* _cardReader;
    SonosChannel* _mainChannel;
    SonosChannel* _secondaryChannel;
    SonosChannel* _configuredMainChannel;
    SonosChannel* _configuredSecondaryChannel;
    std::shared_ptr<RotaryControl> _rotaryControl1 = nullptr;
    std::shared_ptr<Button> _button1 = nullptr;
    std::shared_ptr<RotaryControl> _rotaryControl2 = nullptr;
    std::shared_ptr<Button> _button2 = nullptr;
    std::shared_ptr<RotaryControl> _rotaryControl3 = nullptr;
    std::shared_ptr<Button> _button3 = nullptr;
    std::string _filePathPrefix;
    std::shared_ptr<Card> _currentCard = nullptr;
    uint16_t _pulsingInterval = 909;
    volatile CardReaderState _cardReaderState = CardReaderState::Idle;
    std::vector<Command> _commandsForNextCard;
    std::string readParameterString(uint8_t* parameterValue, int size);
    void setMainChannel(SonosChannel* channel);
    void setSecondaryChannel(SonosChannel* channel);
    void handleCommands(const std::vector<Command>& commands, bool cardRemoved = false);
    void handleButtons();
    void handleLeds();
    void handleRotaryControls();
    void continuePlay();
    void notifyCommandProcessing();
    void initializeRotaryControl(std::shared_ptr<RotaryControl>& rotaryControl, ESP32Encoder& encoder, uint8_t functionSelection, ChannelSelection channelSelection, uint8_t customSonChannel, uint8_t customDevice);
    void initializeButton(std::shared_ptr<Button>& button, uint8_t buttonNumber, uint8_t pin, uint8_t functionSelection, ChannelSelection channelSelection, uint8_t customSonChannel, uint8_t customDevice);

    uint8_t _tempVolumeGroup = 255;
    uint8_t _originalValumeGroup = 255;
    bool _tempTagPlaying = false;
    std::shared_ptr<SonosChannelPlayHandle> _tempVolumePlayHandle = nullptr;
    SonosChannel* _tempVolumeChannel = nullptr;
    void setTempVolumeGroup(uint8_t percentage, std::shared_ptr<SonosChannelPlayHandle>& playHandle);
    void resetFromTempVolumeGroup();
    void handleTempVolume();
    SonosChannel* getChannel(ChannelSelection channelSelection, uint8_t customChannel);
  public:
    const std::string logPrefix() override;
    const std::string name() override;
    const std::string version() override;
    void init() override;
    void loop(bool configured) override;
    void setup(bool configured) override;
    bool processCommand(const std::string cmd, bool debugKo) override;
    void processInputKo(GroupObject &ko) override;
    void showHelp() override;
    bool hasTag() const;
    TagPlayState isPlayingTag() const;
    uint16_t getPulsingInterval() const;
    PlayerState playerState() const;
    CardReaderState cardReaderState() const;
    void setVolumeRelative(ChannelSelection channelSelection, uint8_t customChannel, int8_t diff);
    void setGroupVolumeRelative(ChannelSelection channelSelection, uint8_t customChannel, int8_t diff);
    void togglePlay(ChannelSelection channelSelection, uint8_t customChannel, bool onlyPlay = false);
    void nextTrack(ChannelSelection channelSelection, uint8_t customChannel);
    void previousTrack(ChannelSelection channelSelection, uint8_t customChannel);
};

extern SonosNFCPlayerModule openknxSonosNFCPlayer;
#endif