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
            togglePlay();
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

bool SonosNFCPlayerModule::tryParseDeviceCommand(Command &command, uint8_t &deviceIndex, uint8_t &percentage, uint8_t &onOff)
{
    deviceIndex = -1;
    percentage = -1;
    onOff = -1;
    if (command.name.rfind("device", 0) != 0)
    {
        return false;
    }
    std::string deviceIndexStr = command.name.substr(6);
    if (deviceIndexStr.empty())
    {
        logDebugP("Command '%s' does not contain device index", command.name.c_str());
        return false;
    }
    try
    {
        auto device = std::stoi(deviceIndexStr);
        if (device < 1)
            return false;
        if (device > ParamDEV_VisibleChannels)
            return false;
        deviceIndex = device - 1;
        std::string lowerCaseParameter;
        for (char c : command.parameter)
            lowerCaseParameter += std::tolower(c);
        if (lowerCaseParameter == "on")
        {
            onOff = 1;
        }
        else if (lowerCaseParameter == "off")
        {
            onOff = 0;
        }
        else
        {
            auto percentageValue = std::stoi(lowerCaseParameter);
            if (percentageValue < 0 || percentageValue > 100)
                return false;
            percentage = percentageValue;
        }
    }
    catch (const std::exception &)
    {
        return false; // keine gültige Zahl
    }
    return true;
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
            _pulsingInterval = _currentCard->hasCommand("SINGLE") ? 667 : 909;
            _currentPlayHandle = nullptr;
            logInfoP("New card detected");
            logIndentUp();
            _currentCard->logInformation();
            logIndentDown();
            KoPLY_Card.value(true, DPT_Switch);
            KoPLY_CardId.value(_currentCard->getUid().c_str(), DPT_String_ASCII);
            handleLeds();
            handleCommands(_currentCard->getCommands());
        }
        else
        {
            logInfoP("Card removed");
            KoPLY_Card.value(false, DPT_Switch);
            KoPLY_CardId.value("", DPT_String_ASCII);
            handleLeds();
            handleCommands(_currentCard->getCommands(), true);
        }
    }
    if (!_playAllowed && _cardReaderState == CardReaderState::CARD_READER_STATE_AVAILABLE || _cardReaderState == CardReaderState::CARD_READER_STATE_IDLE)
    {
        _playAllowed = true;
    }
}

void SonosNFCPlayerModule::handleCommands(const std::vector<Command> &commands, bool cardRemoved)
{
    const std::vector<Command> *currentCommands = &commands;
    std::vector<Command> newCommands = {};
    if (!cardRemoved)
    {
        // Handle commands from previous card
        if (_commandsForNextCard.size() > 0)
        {
            for (auto command : commands)
            {
                if (command.name.rfind("#", 0) == 0)
                    continue; // skip commands for next card
                bool exists = false;
                for (auto existingCommand : _commandsForNextCard)
                {
                    if (existingCommand.name == command.name)
                    {
                        exists = true;
                    }
                }
                if (!exists)
                    newCommands.push_back(command);
            }
            for (auto command : _commandsForNextCard)
            {
                if (command.name == command.name)
                {
                    Command newCommand = command;
                    newCommand.name = command.name.substr(1); // remove '#' prefix
                    logDebugP("Adding command from previous card: %s", newCommand.name.c_str());
                    newCommands.push_back(newCommand);
                }
            }
            currentCommands = &newCommands;
            _commandsForNextCard.clear();
        }
        // Store commands for next card
        for (auto command : commands)
        {
            if (command.name.rfind("#", 0) == 0)
                _commandsForNextCard.push_back(command);
        }
    }
    std::string uri = "";
    std::string title = "";
    std::string image = "";
    bool pause = false;
    bool continuePlaying = false;
    if (cardRemoved && ParamPLY_StopOnRemoveTag)
    {
        pause = true;
    }
    for (auto command : *currentCommands)
    {
        std::string name = cardRemoved ? (">" + command.name) : command.name;
        if (_playAllowed)
        {
            uint8_t deviceIndex = -1;
            uint8_t percentage = -1;
            uint8_t onOff = -1;
            if (tryParseDeviceCommand(command, deviceIndex, percentage, onOff))
            {
                if (percentage != -1)
                {
                    auto _channelIndex = deviceIndex;
                    logDebugP("Setting device with index %d to %d%%", deviceIndex, percentage);
                    KoDEV_CHPercentage.value(percentage, DPT_Scaling);
                }
                if (onOff != -1)
                {
                    auto _channelIndex = deviceIndex;
                    logDebugP("Setting device with index %d to %s", deviceIndex, onOff == 1 ? "ON" : "OFF");
                    KoDEV_CHSwitch.value(onOff == 1, DPT_Switch);
                }
            }
            else if (name == "shuffle" && _mainChannel != nullptr)
            {
                _mainChannel->shuffle(command.getParameterAsBool(true));
            }
        }
        if (name == "uri")
        {
            uri = command.parameter;
        }
        else if (name == "title")
        {
            title = command.parameter;
        }
        else if (name == "image")
        {
            image = command.parameter;
        }
        else if (name == "pause" || name == "stop")
        {
            pause = command.getParameterAsBool(true);
        }
        else if (name == "continue")
        {
            continuePlaying = command.getParameterAsBool(true);
        }
    }
    if (!uri.empty())
    {
        _currentPlayHandle = _mainChannel->start(uri.c_str(), title.c_str(), image.c_str(), _filePathPrefix.c_str(), _playAllowed);
        if (_secondaryChannel != nullptr && _playAllowed)
            _secondaryChannel->joinToGroupCoordinator(_mainChannel);
    }
    else if (pause && _playAllowed)
    {
        if (_mainChannel != nullptr && ParamPLY_StopOnRemoveTag && _mainChannel->isPlaying(_currentPlayHandle))
        {
            _mainChannel->pause();
        }
    }
    else if (continuePlaying && _playAllowed)
    {
        continuePlay();
    }
}

void SonosNFCPlayerModule::continuePlay()
{
    togglePlay(true);
}

void SonosNFCPlayerModule::togglePlay(bool onlyPlay)
{
    if (_mainChannel != nullptr)
    {
        switch (_mainChannel->getPlayState())
        {
        case SonosApiPlayState::Paused_Playback:
            _mainChannel->play();
            break;
        case SonosApiPlayState::Stopped:
            _mainChannel->start(_currentPlayHandle);
             break;
        case SonosApiPlayState::Transitioning:
        case SonosApiPlayState::Playing:
            if (!onlyPlay)
                _mainChannel->pause();
            break;
        default:
            break;
        }
    }
}

void SonosNFCPlayerModule::handleLeds()
{
    _ledFunctionPlayerState.loop();
    _ledFunctionTag.loop();
}

void SonosNFCPlayerModule::processInputKo(GroupObject &ko)
{
    logDebugP("Input KO %d changed", (int)ko.asap());
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