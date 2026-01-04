#include "SonosNFCPlayerModule.h"
#include "SonosModule.h"
#include "SonosChannel.h"
#include "SonosVolumeController.h"
#include "SonosGroupVolumeController.h"
#include <ESP32Encoder.h>

SonosNFCPlayerModule openknxSonosNFCPlayer;

const std::string SonosNFCPlayerModule::logPrefix()
{
    return "Player";
}

const std::string SonosNFCPlayerModule::name()
{
    return "Sonos NFCP Payer";
}

// You can also give it a version
// will be displayed in Command Infos
const std::string SonosNFCPlayerModule::version()
{
    return std::to_string(MAIN_ApplicationVersion);
}

void SonosNFCPlayerModule::init()
{
}

void SonosNFCPlayerModule::setup(bool configured)
{
    _cardReader = new CardReader();
    _cardReader->setup();
    if (configured)
    {
        _filePathPrefix = readParameterString(ParamPLY_FileSharePrefix, 50);
        _mainChannel = openknxSonosModule.getChannel(ParamPLY_MainChannel - 1);
        if (_mainChannel == nullptr)
        {
            logErrorP("Main channel %d not active", ParamPLY_MainChannel);
        }
        if (ParamPLY_SecondaryChannel > 0)
        {
            _secondaryChannel = openknxSonosModule.getChannel(ParamPLY_SecondaryChannel - 1);
            if (_secondaryChannel == nullptr)
            {
                logErrorP("Secondary channel %d not active", ParamPLY_SecondaryChannel);
            }
        }
        // init encoder and buttons

       
        pinMode(ENCODER1_BUTTON, INPUT_PULLUP);
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 1");
        _encoder1.attachHalfQuad(ENCODER1_DT, ENCODER1_CLK);
        _encoder1.setCount(0);
       
        pinMode(ENCODER2_BUTTON, INPUT_PULLUP);
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 2");
        _encoder2.attachHalfQuad(ENCODER2_DT, ENCODER2_CLK);
        _encoder2.setCount(0);

        pinMode(ENCODER3_BUTTON, INPUT_PULLUP);
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 3");
        _encoder2.attachHalfQuad(ENCODER3_DT, ENCODER3_CLK);
        _encoder2.setCount(0);


        // init sonos volume
        if (_mainChannel != nullptr)
        {
        //     _sonosGroupVolumeController = new SonosGroupVolumeController(_encoder1, _mainChannel);
        //     _sonosVolumeController1 = new SonosVolumeController(_encoder2, _mainChannel);
        }
        if (_secondaryChannel != nullptr)
        {
          //  _sonosVolumeController2 = new SonosVolumeController(_encoder3, _secondaryChannel);
        }
    }
}

void SonosNFCPlayerModule::handleButtons()
{
    if (openknxSonosModule.isInitialized() == false)
        return;
    bool input = !digitalRead(ENCODER1_BUTTON);
    if (_lastButtonState1 != input)
    {
        _lastButtonState1 = input;
        logDebugP("Pressed 1 %d", input);
        if (input)
        {
            if (_mainChannel != nullptr)
                _mainChannel->togglePause();
        }
    }

    input = !digitalRead(ENCODER2_BUTTON);
    if (_lastButtonState2 != input)
    {
        _lastButtonState2 = input;
        logDebugP("Pressed 2 %d", input);
        Serial.println(input);
        if (input)
        {
            //   ledStrip.forceOn(true);
            //   ledStrip.loop();
            _mainChannel->previousTrack();
            // ledStrip.forceOn(false);
        }
    }

    input = !digitalRead(ENCODER3_BUTTON);
    if (_lastButtonState3 != input)
    {
        _lastButtonState3 = input;
        logDebugP("Pressed 3 %d", input);
        if (input)
        {
            //   ledStrip.forceOn(true);
            //   ledStrip.loop();
            if (_mainChannel != nullptr)
                _mainChannel->nextTrack();
            // ledStrip.forceOn(false);
        }
    }
}

std::string SonosNFCPlayerModule::readParameterString(uint8_t *parameterValue, int size)
{
    return std::string((const char *)parameterValue, strnlen((const char *)parameterValue, size));
}

void SonosNFCPlayerModule::loop(bool configured)
{
    handleLeds();
    handleButtons();
    if (_cardReader == nullptr)
        return;
    if (openknxSonosModule.isInitialized() == false)
        return;
    if (_sonosGroupVolumeController != nullptr)
        _sonosGroupVolumeController->loop();
    if (_sonosVolumeController1 != nullptr)
        _sonosVolumeController1->loop();
    if (_sonosVolumeController2 != nullptr)
        _sonosVolumeController2->loop();

    auto cardReaderState = _cardReader->state();
    if (cardReaderState != _cardReaderState)
    {
        _cardReaderState = cardReaderState;
        logDebugP("Card reader changed: %d", (int)cardReaderState);
    }
    auto currentCard = _cardReaderState == CardReaderState::CARD_READER_STATE_AVAILABLE ? _cardReader->currentCard() : nullptr;
    if (currentCard != _currentCard)
    {
        _currentCard = currentCard;
        if (_currentCard != nullptr)
        {
            _pulsingInterval = _currentCard->hasCommand("SINGLE") ?  667 : 909;
            _currentPlayHandle = nullptr;
            logInfoP("New card detected");
            logIndentUp();
            _currentCard->logInformation();
            logIndentDown();
            KoPLY_Card.value(true, DPT_Switch);
            KoPLY_CardId.value(_currentCard->getUid().c_str(), DPT_String_ASCII);
            handleLeds();
            if (_mainChannel != nullptr)
            {
                if (_playAllowed)
                {
                    if (_currentCard->hasCommand("SHUFFLE"))
                        _mainChannel->shuffle(true);
                    else
                        _mainChannel->shuffle(false);

                    _currentPlayHandle = _mainChannel->start(_currentCard->getUrl().c_str(), _currentCard->getTitle().c_str(), _currentCard->getImageUrl().c_str(), _filePathPrefix.c_str(), true);
                    if (_secondaryChannel != nullptr)
                        _secondaryChannel->joinToGroupCoordinator(_mainChannel);
                }
                else
                {
                    logWarningP("Play not allowed yet, starting up");
                    _currentPlayHandle = _mainChannel->start(_currentCard->getUrl().c_str(), _currentCard->getTitle().c_str(), _currentCard->getImageUrl().c_str(), _filePathPrefix.c_str(), false);
          
                }
            }
        }
        else
        {
            logInfoP("Card removed");
            KoPLY_Card.value(false, DPT_Switch);
            KoPLY_CardId.value("", DPT_String_ASCII);
            handleLeds();
            if (_mainChannel != nullptr && ParamPLY_StopOnRemoveTag)
                _mainChannel->pause();
        }
    }
    if (!_playAllowed && _cardReaderState == CardReaderState::CARD_READER_STATE_AVAILABLE || _cardReaderState == CardReaderState::CARD_READER_STATE_IDLE)
    {
        _playAllowed = true;
    }
  
}

void SonosNFCPlayerModule::handleLeds()
{
    _ledFunctionPlayerState.loop();
    _ledFunctionTag.loop();
}

bool SonosNFCPlayerModule::processCommand(const std::string cmd, bool debugKo)
{
    if (cmd == "ply")
    {
        if (_cardReader == nullptr)
        {
            logErrorP("Card reader not initialized");
        }
        else
        {
            _cardReader->logInformation();
        }
        if (_currentCard != nullptr)
        {
            _currentCard->logInformation();
        }
        else
        {
            logInfoP("No card present");
        }
        return true;
    }
    return false;
}

void SonosNFCPlayerModule::showHelp()
{
    openknx.console.printHelpLine("ply", "Show player informations");
}

bool SonosNFCPlayerModule::hasTag() const
{
    return _cardReaderState == CardReaderState::CARD_READER_ERROR ||
           _cardReaderState == CardReaderState::CARD_READER_STATE_TAG_READING ||
           _cardReaderState == CardReaderState::CARD_READER_STATE_AVAILABLE;
}

bool SonosNFCPlayerModule::isPlayingTag() const
{
    if (_mainChannel == nullptr)
        return false;
    return _mainChannel->isPlaying(_currentPlayHandle);
}

CardReaderState SonosNFCPlayerModule::cardReaderState() const
{
    return _cardReaderState;
}

uint16_t SonosNFCPlayerModule::getPulsingInterval() const
{
    return _pulsingInterval;
}