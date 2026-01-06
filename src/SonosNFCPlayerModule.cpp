#include "SonosNFCPlayerModule.h"
#include "SonosModule.h"
#include "SonosChannel.h"
#include "SonosVolumeController.h"
#include "SonosGroupVolumeController.h"
#include <ESP32Encoder.h>
#include "PlayerState.h"

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
        _configuredMainChannel = openknxSonosModule.getChannel(ParamPLY_MainChannel - 1);
        if (_configuredMainChannel == nullptr)
            logErrorP("Main channel %d not active", ParamPLY_MainChannel);
        _mainChannel = _configuredMainChannel;
        if (ParamPLY_SecondaryChannel > 0)
        {
            _configuredSecondaryChannel = openknxSonosModule.getChannel(ParamPLY_SecondaryChannel - 1);
            if (_configuredSecondaryChannel == nullptr)
                logErrorP("Secondary channel %d not active", ParamPLY_SecondaryChannel);
            _secondaryChannel = _configuredSecondaryChannel;
        }
        // init encoder and buttons

        pinMode(ENCODER1_BUTTON, INPUT_PULLUP);
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 1");
        _encoder1.attachHalfQuad(ENCODER1_DT, ENCODER1_CLK);
      
        pinMode(ENCODER2_BUTTON, INPUT_PULLUP);
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 2");
        _encoder2.attachHalfQuad(ENCODER2_DT, ENCODER2_CLK);
      
        pinMode(ENCODER3_BUTTON, INPUT_PULLUP);
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 3");
        _encoder3.attachHalfQuad(ENCODER3_DT, ENCODER3_CLK);
      
        // init sonos volume
        if (_mainChannel != nullptr)
        {
            _sonosGroupVolumeController = new SonosGroupVolumeController(_encoder1, _mainChannel);
            _sonosVolumeController1 = new SonosVolumeController(_encoder2, _mainChannel);
        }
        if (_secondaryChannel != nullptr)
        {
            _sonosVolumeController2 = new SonosVolumeController(_encoder3, _secondaryChannel);
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


void SonosNFCPlayerModule::loop(bool configured)
{
    if (_lastCommandProcessTime != 0 && _lastCommandProcessTime + 1000 > millis())
    {
        _lastCommandProcessTime = 0;
    }
    handleLeds();
    handleButtons();
    handleTempVolume();
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
    auto currentCard = _cardReaderState == CardReaderState::Available ? _cardReader->currentCard() : nullptr;
    if (currentCard != _currentCard)
    {
        auto previousCard = _currentCard;
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
            if (previousCard != nullptr)
                handleCommands(previousCard->getCommands(), true);
            handleCommands(_currentCard->getCommands());
        }
        else
        {
            handleLeds();
            if (previousCard != nullptr)
                handleCommands(previousCard->getCommands(), true);
            logInfoP("Card removed");
            KoPLY_Card.value(false, DPT_Switch);
            KoPLY_CardId.value("", DPT_String_ASCII);
           
        }
    }
    if (!_playAllowed && _cardReaderState == CardReaderState::Available || _cardReaderState == CardReaderState::Idle)
    {
        _playAllowed = true;
    }
}

void SonosNFCPlayerModule::handleCommands(const std::vector<Command> &commands, bool cardRemoved)
{
    const std::vector<Command> *currentCommands = &commands;
    std::vector<Command> newCommands = {};
    bool hasPlayCommand = false;
    for (auto& command : commands)
    {
        if (command.name == "uri" && 
            !command.parameter.empty())
        {
            hasPlayCommand = true;
            break;
        }
    }
    if (cardRemoved)
    {
        resetFromTempVolumeGroup();
    }
    else
    {
        // Handle commands from previous card
        if (_commandsForNextCard.size() > 0)
        {
            if (!hasPlayCommand) notifyCommandProcessing();
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
                    newCommands.push_back(command);
                }
            }
            currentCommands = &newCommands;
            _commandsForNextCard.clear();
        }
        // Store commands for next card
        for (auto command : commands)
        {
            if (command.name.rfind("#", 0) == 0)
            {
                auto newCommand = command;
                newCommand.name = command.name.substr(1); // remove '#' prefix
                _commandsForNextCard.push_back(newCommand);
            }
        }
    }

    std::string uri = "";
    std::string title = "";
    std::string image = "";
    uint8_t tempPercentage = 255;
          
    bool pause = false;
    bool continuePlaying = false;
    if (cardRemoved && ParamPLY_StopOnRemoveTag)
    {
        pause = true;
    }
    for (Command command /* do not use reference, we make modifications */ : *currentCommands)
    {
        if (cardRemoved)
        {
            if (command.name.rfind('>') == 0)
                command.name = command.name.substr(1);
            else
            {
                logDebugP("Skipping command %s on card removal", command.name.c_str());
                continue;
            }
        }
        else if (command.name.rfind('>') == 0)
        {
            logDebugP("Skipping command %s on card placement", command.name.c_str());
            continue;
        }
        auto& name = command.name;
        if (_playAllowed)
        {
            uint8_t deviceIndex = 255;
            uint8_t percentage = 255;
            uint8_t onOff = 255;
           
            if (command.isNameWithIndex("device", deviceIndex) && command.tryGetParameterAsPercentageOrBool(percentage, onOff))
            {
                if (deviceIndex < 1 || deviceIndex > ParamDEV_VisibleChannels)
                {
                    logWarningP("Device index %d is out of range", deviceIndex);
                    continue;
                }
                if (percentage != 255)
                {
                    if (!hasPlayCommand) notifyCommandProcessing();
                    auto _channelIndex = deviceIndex - 1;
                    logDebugP("Setting device with index %d to %d%%, KO: %d", deviceIndex, percentage, (int) KoDEV_CHPercentage.asap());
                    KoDEV_CHPercentage.value(percentage, DPT_Scaling);
                }
                if (onOff != 255)
                {
                   if (!hasPlayCommand) notifyCommandProcessing();
                    auto _channelIndex = deviceIndex - 1;
                    logDebugP("Setting device with index %d to %s, KO: %d", deviceIndex, onOff == 1 ? "ON" : "OFF", (int) KoDEV_CHSwitch.asap());
                    KoDEV_CHSwitch.value(onOff == 1, DPT_Switch);
                }
            }
            else
            {
                uint8_t percentage = 0;
                uint8_t channelNumber = 0;
                if (command.name == "templedinterval")
                {
                    int value;
                    if (command.tryGetParameterAsInt(value))
                    {
                        if (value > 0)
                        {
                            _pulsingInterval = value;
                            logDebugP("Setting LED pulsing interval to %d ms", _pulsingInterval);
                        }
                    }
                    else if (!cardRemoved)
                    {
                        _pulsingInterval = 909;
                        logDebugP("Reset LED pulsing interval to %d ms", _pulsingInterval);
                    }
                }
                else if (command.name == "mainspeaker")
                {
                    if (command.parameter.empty())
                    {
                        _mainChannel = _configuredMainChannel;
                        logDebugP("Setting main speaker to configured channel");
                         if (!hasPlayCommand) notifyCommandProcessing();
                    }
                    else if (command.tryGetParameterAsPercent(channelNumber))
                    {
                        auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                        if (channel != nullptr)
                        {
                            _mainChannel = channel;
                            logDebugP("Setting main speaker to %d", channelNumber);
                            if (!hasPlayCommand) notifyCommandProcessing();
                        }
                        else
                            logWarningP("Sonsos channel %d not found for speaker command", channelNumber);
                    }
                   
                }
                else if (command.name == "secondaryspeaker")
                {
                    if (command.parameter.empty())
                    {
                        _secondaryChannel = _configuredSecondaryChannel;
                        logDebugP("Setting secondary speaker to configured channel");
                        if (!hasPlayCommand) notifyCommandProcessing();
                    }
                    else if (command.tryGetParameterAsPercent(channelNumber))
                    {
                        if (channelNumber == 0)
                        {
                            _secondaryChannel = nullptr;
                            logDebugP("Disabling secondary speaker");
                            if (!hasPlayCommand) notifyCommandProcessing();
                        }
                        else
                        {
                            auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                            if (channel != nullptr)
                            {
                                _secondaryChannel = channel;
                                logDebugP("Setting secondary speaker to %d", channelNumber);
                                if (!hasPlayCommand) notifyCommandProcessing();
                            }
                            else
                                logWarningP("Sonsos channel %d not found for speaker command", channelNumber);
                        }
                    }
                }
                else if (_mainChannel != nullptr)
                {
                    if (name == "shuffle")
                    {
                        if (!hasPlayCommand) notifyCommandProcessing();
                        _mainChannel->shuffle(command.getParameterAsBool(true));
                    }
                    else if (name == "volume" && command.tryGetParameterAsPercent(percentage))
                    {
                        if (!hasPlayCommand) notifyCommandProcessing();
                        _mainChannel->setVolume(percentage);
                    }
                    else if (name == "volumegroup" && command.tryGetParameterAsPercent(percentage))
                    {
                        if (!hasPlayCommand) notifyCommandProcessing();
                        _mainChannel->setGroupVolume(percentage);
                    }
                    else if (name == "tempvolumegroup" && command.tryGetParameterAsPercent(tempPercentage))
                    {
                        // will be handled in starting playing
                    }
                    else if (command.isNameWithIndex("volume", channelNumber))
                    {
                        auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                        if (channel != nullptr)
                        {
                            if (command.tryGetParameterAsPercent(percentage))
                            {
                                if (!hasPlayCommand) notifyCommandProcessing();
                                channel->setVolume(percentage);
                            }
                        }
                        else
                        {
                            logWarningP("Sonsos channel %d not found for volume command", channelNumber);
                        }
                    }
                    else if (command.isNameWithIndex("volumegroup", channelNumber))
                    {
                        auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                        if (channel != nullptr)
                        {
                            if (command.tryGetParameterAsPercent(percentage))
                            {
                                if (!hasPlayCommand) notifyCommandProcessing();
                                channel->setGroupVolume(percentage);
                            }
                        }
                        else
                        {
                            logWarningP("Sonsos channel %d not found for volume command", channelNumber);
                        }
                    }
                    else if (command.name == "join")
                    {
                        uint8_t channelNumber = 0;
                        if (command.tryGetParameterAsPercent(channelNumber))
                        {
                            auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                            if (channel != nullptr)
                            {
                                if (!hasPlayCommand) notifyCommandProcessing();
                                channel->joinToGroupCoordinatorOf(_mainChannel);
                            }
                           
                        }
                        else
                        {
                            logWarningP("Parameter for join command is not valid: %s", command.parameter.c_str());
                        }
                    }
                    else if (command.name == "unjoin")
                    {
                        uint8_t channelNumber = 0;
                        if (command.tryGetParameterAsPercent(channelNumber))
                        {
                            auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                            if (channel != nullptr)
                            {
                                if (!hasPlayCommand) notifyCommandProcessing();
                                channel->unjoin();
                            }
                           
                        }
                        else
                        {
                            logWarningP("Parameter for unjoin command is not valid: %s", command.parameter.c_str());
                        }
                    }
                }
                if (_secondaryChannel != nullptr)
                {
                    if (name == "volumesecondary" && command.tryGetParameterAsPercent(percentage))
                    {
                        if (!hasPlayCommand) notifyCommandProcessing();
                            _secondaryChannel->setVolume(percentage);
                    }
                }
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
        if (tempPercentage != 255)
        {
            setTempVolumeGroup(tempPercentage, _currentPlayHandle);
        }
        if (_secondaryChannel != nullptr && _playAllowed)
            _secondaryChannel->joinToGroupCoordinatorOf(_mainChannel);
    }
    else if (pause && _playAllowed)
    {
        if (_mainChannel != nullptr && ParamPLY_StopOnRemoveTag && _mainChannel->isPlaying(_currentPlayHandle.get()))
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

void SonosNFCPlayerModule::notifyCommandProcessing()
{
    _lastCommandProcessTime = max(1UL, millis());
    handleLeds();
}

void SonosNFCPlayerModule::handleLeds()
{
    _ledFunctionPlayerState.loop();
    _ledFunctionTag.loop();
}

void SonosNFCPlayerModule::setTempVolumeGroup(uint8_t percentage, std::shared_ptr<SonosChannelPlayHandle>& playingCheck)
{
    if (_tempVolumeChannel == nullptr)
    {
        if (_mainChannel == nullptr)
            return;
        _tempVolumeGroup = percentage;
        _tempVolumePlayHandle = playingCheck;

        _originalValumeGroup = _mainChannel->getGroupVolume();
        _tempVolumeChannel = _mainChannel;
        _tempTagPlaying = false;
    }
    else
    {
        _tempVolumeGroup = percentage;
        _tempVolumePlayHandle = playingCheck;
        _tempTagPlaying = false;
    }
}

void SonosNFCPlayerModule::resetFromTempVolumeGroup()
{
    if (_tempVolumeChannel != nullptr)
    {
        logDebugP("Restoring from temp volume group and set to %d", _originalValumeGroup);
        _tempVolumeChannel->setGroupVolume(_originalValumeGroup);
        _tempVolumeChannel = nullptr;
        _tempVolumeGroup = 255;
        _originalValumeGroup = 255;
        _tempTagPlaying = false;
    }
}

void SonosNFCPlayerModule::handleTempVolume()
{
    if (_tempVolumeChannel != nullptr)
    {
        bool isPlaying = _tempVolumePlayHandle == nullptr || _tempVolumePlayHandle->isPlaying();
        if (isPlaying == _tempTagPlaying)
            return;
        _tempTagPlaying = isPlaying;
        if (isPlaying)
        {
            logDebugP("Temp volume handling: Tag playing state changed to %d", isPlaying);
            _tempVolumeChannel->setGroupVolume(_tempVolumeGroup);
        }
        else
        {
            logDebugP("Temp volume handling: Tag playing state changed to %d", isPlaying);
            _tempVolumeChannel->setGroupVolume(_originalValumeGroup);
        }
    }
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
    return _cardReaderState == CardReaderState::Error ||
           _cardReaderState == CardReaderState::TagReading ||
           _cardReaderState == CardReaderState::Available;
}

bool SonosNFCPlayerModule::isPlayingTag() const
{
    if (_mainChannel == nullptr)
        return false;
    return _mainChannel->isPlaying(_currentPlayHandle.get());
}

CardReaderState SonosNFCPlayerModule::cardReaderState() const
{
    return _cardReaderState;
}

PlayerState SonosNFCPlayerModule::playerState() const
{
    if (_lastCommandProcessTime != 0)
    {
        return PlayerState::CommandProcessing;
    }
    switch (_cardReaderState)
    {
    case CardReaderState::Error:
        return PlayerState::CardReaderError;
    case CardReaderState::Initializing:
        return PlayerState::CardReaderInitializing;
    case CardReaderState::TagReading:
        return PlayerState::CardReaderTagReading;
    case CardReaderState::Available:
        return PlayerState::CardReaderAvailable;
    default:
        if (isPlayingTag())
           return PlayerState::PlayingTag;
        return PlayerState::Idle;
    }
}

uint16_t SonosNFCPlayerModule::getPulsingInterval() const
{
    return _pulsingInterval;
}