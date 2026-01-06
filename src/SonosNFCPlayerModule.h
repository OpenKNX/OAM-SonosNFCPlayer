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

class SonosChannel;
class SonosVolumeController;
class SonosGroupVolumeController;

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

    bool _playAllowed = false;
    
    unsigned  long _lastCommandProcessTime = 0;
    CardReader* _cardReader;
    SonosChannel* _mainChannel;
    SonosChannel* _secondaryChannel;
    SonosChannel* _configuredMainChannel;
    SonosChannel* _configuredSecondaryChannel;
    SonosGroupVolumeController* _sonosGroupVolumeController = nullptr;
    SonosVolumeController* _sonosVolumeController1= nullptr;
    SonosVolumeController* _sonosVolumeController2 = nullptr;
    std::string _filePathPrefix;
    std::shared_ptr<Card> _currentCard = nullptr;
    uint16_t _pulsingInterval = 909;
    volatile CardReaderState _cardReaderState = CardReaderState::Idle;
    std::vector<Command> _commandsForNextCard;
    std::string readParameterString(uint8_t* parameterValue, int size);
    void handleCommands(const std::vector<Command>& commands, bool cardRemoved = false);
    void handleButtons();
    void handleLeds();
    void continuePlay();
    void togglePlay(bool onlyPlay = false);
    void notifyCommandProcessing();

    uint8_t _tempVolumeGroup = 255;
    uint8_t _originalValumeGroup = 255;
    bool _tempTagPlaying = false;
    std::shared_ptr<SonosChannelPlayHandle> _tempVolumePlayHandle = nullptr;
    SonosChannel* _tempVolumeChannel = nullptr;
    void setTempVolumeGroup(uint8_t percentage, std::shared_ptr<SonosChannelPlayHandle>& playHandle);
    void resetFromTempVolumeGroup();
    void handleTempVolume();
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
    bool isPlayingTag() const;
    uint16_t getPulsingInterval() const;
    PlayerState playerState() const;
    CardReaderState cardReaderState() const;

};

extern SonosNFCPlayerModule openknxSonosNFCPlayer;
#endif