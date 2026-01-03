#if defined(KNX_IP_WIFI) || defined(KNX_IP_LAN)
#pragma once
#include "OpenKNX.h"
#include "CardReader.h"
#include "string"
#include <ESP32Encoder.h>
#include "LedFunctionPlayerState.h"
#include "LedFunctionTag.h"

class SonosChannel;
class SonosVolumeController;
class SonosGroupVolumeController;

class SonosNFCPlayerModule : public OpenKNX::Module
{
    bool _lastButtonState1 = false;
    bool _lastButtonState2 = false;
    bool _lastButtonState3 = false;
    ESP32Encoder _encoder1;
    ESP32Encoder _encoder2;
    ESP32Encoder _encoder3;
    LedFunctionPlayerState _ledFunctionPlayerState;
    LedFunctionTag _ledFunctionTag;

    bool _playAllowed = false;
    CardReader* _cardReader;
    SonosChannel* _mainChannel;
    SonosChannel* _secondaryChannel;
    SonosGroupVolumeController* _sonosGroupVolumeController = nullptr;
    SonosVolumeController* _sonosVolumeController1= nullptr;
    SonosVolumeController* _sonosVolumeController2 = nullptr;
    std::string _filePathPrefix;
    std::shared_ptr<Card> _currentCard = nullptr;
    CardReaderState _cardReaderState = CardReaderState::CARD_READER_STATE_IDLE;
    std::string readParameterString(uint8_t* parameterValue, int size);
  public:
    const std::string logPrefix() override;
    const std::string name() override;
    const std::string version() override;
    void init() override;
    void loop(bool configured) override;
    void setup(bool configured) override;
    bool processCommand(const std::string cmd, bool debugKo);
    void showHelp() override;
    void handleButtons();
    bool hasTag() const;

};

extern SonosNFCPlayerModule openknxSonosNFCPlayer;
#endif