#include "OpenKNX.h"
#include "SonosNFCPlayerModule.h"
#include "SonosModule.h"
#include "SonosChannel.h"
#include "RotaryControlVolume.h"
#include "RotaryControlGroupVolume.h"
#include "RotaryControlDevice.h"
#include "ButtonPause.h"
#include "ButtonNextTrack.h"
#include "ButtonPreviousTrack.h"
#include "ButtonDevice.h"
#include <ESP32Encoder.h>
#include "PlayerState.h"
#include "NetworkModule.h"

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
        if (!KoPLY_MainChannelStatus.initialized())
            KoPLY_MainChannelStatus.requestObjectRead();
        if (!KoPLY_SecondaryChannelStatus.initialized())
            KoPLY_SecondaryChannelStatus.requestObjectRead();
 
        _filePathPrefix = readParameterString(ParamPLY_FileSharePrefix, 50);
        _configuredMainChannel = openknxSonosModule.getChannel(ParamPLY_MainChannel - 1);
        if (_configuredMainChannel == nullptr)
            logErrorP("Main channel %d not active", ParamPLY_MainChannel);
        setMainChannel(_configuredMainChannel);
        if (ParamPLY_SecondaryChannel > 0)
        {
            _configuredSecondaryChannel = openknxSonosModule.getChannel(ParamPLY_SecondaryChannel - 1);
            if (_configuredSecondaryChannel == nullptr)
                logErrorP("Secondary channel %d not active", ParamPLY_SecondaryChannel);
            setSecondaryChannel(_configuredSecondaryChannel);
        }
        // init encoder and buttons

        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 1");
        _encoder1.attachHalfQuad(ENCODER1_DT, ENCODER1_CLK);
      
        initializeRotaryControl(_rotaryControl1, _encoder1, ParamPLY_RotaryController1, (ChannelSelection) ParamPLY_RotaryController1ChSelect, (uint8_t) ParamPLY_RotaryController1Son, (uint8_t) ParamPLY_RotaryController1Dev);
        initializeButton(_button1, 1, ENCODER1_BUTTON, ParamPLY_Button1, (ChannelSelection) ParamPLY_Button1ChSelect, (uint8_t) ParamPLY_Button1Son, (uint8_t) ParamPLY_Button1Dev);
       
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 2");
        _encoder2.attachHalfQuad(ENCODER2_DT, ENCODER2_CLK);

        initializeRotaryControl(_rotaryControl2, _encoder2, ParamPLY_RotaryController2, (ChannelSelection) ParamPLY_RotaryController2ChSelect, (uint8_t) ParamPLY_RotaryController2Son, (uint8_t) ParamPLY_RotaryController2Dev);
        initializeButton(_button2, 2, ENCODER2_BUTTON, ParamPLY_Button2, (ChannelSelection) ParamPLY_Button2ChSelect, (uint8_t) ParamPLY_Button2Son, (uint8_t) ParamPLY_Button2Dev);

      
        ESP32Encoder::useInternalWeakPullResistors = puType::up;
        logDebugP("Attaching encoder 3");
        _encoder3.attachHalfQuad(ENCODER3_DT, ENCODER3_CLK);

        initializeRotaryControl(_rotaryControl3, _encoder3, ParamPLY_RotaryController3, (ChannelSelection) ParamPLY_RotaryController3ChSelect, (uint8_t) ParamPLY_RotaryController3Son, (uint8_t) ParamPLY_RotaryController3Dev);
        initializeButton(_button3, 3, ENCODER3_BUTTON, ParamPLY_Button3, (ChannelSelection) ParamPLY_Button3ChSelect, (uint8_t) ParamPLY_Button3Son, (uint8_t) ParamPLY_Button3Dev);
    }
}

void SonosNFCPlayerModule::setMainChannel(SonosChannel* channel)
{
    _mainChannel = channel;
    KoPLY_MainChannelStatus.value(channel->getChannelIndex(), DPT_Value_1_Ucount);
}

void SonosNFCPlayerModule::setSecondaryChannel(SonosChannel* channel)
{
    _secondaryChannel = channel;
    KoPLY_SecondaryChannelStatus.value(channel == nullptr ? (uint8_t) 63 : channel->getChannelIndex(), DPT_Value_1_Ucount);
}

void SonosNFCPlayerModule::initializeRotaryControl(std::shared_ptr<RotaryControl>& rotaryControl, ESP32Encoder& encoder, uint8_t functionSelection, ChannelSelection channelSelection, uint8_t customSonChannel, uint8_t customDevice)
{
    switch (functionSelection)
    {
        case 0:
            logDebugP("Initializing rotary control as volume control");
            rotaryControl =  std::make_shared<RotaryControlVolume>(*this, encoder, channelSelection, customSonChannel);
            break;
        case 1:
            logDebugP("Initializing rotary control as group volume control");
            rotaryControl =  std::make_shared<RotaryControlGroupVolume>(*this, encoder, channelSelection, customSonChannel);
            break;
        case 2:
            logDebugP("Initializing rotary control as device percentage control");
            rotaryControl =  std::make_shared<RotaryControlDevice>(*this, encoder, customDevice);
            break;
    }
}

void SonosNFCPlayerModule::initializeButton(std::shared_ptr<Button>& button, uint8_t buttonNumber, uint8_t pin, uint8_t functionSelection, ChannelSelection channelSelection, uint8_t customSonChannel, uint8_t customDevice)
{
    switch (functionSelection)
    {
        case 0:
            logDebugP("Initializing button %d as pause/play", buttonNumber);
            button = std::make_shared<ButtonPause>(*this, pin, channelSelection, customSonChannel);
            break;
        case 1:
            logDebugP("Initializing button %d as next track", buttonNumber);
            button = std::make_shared<ButtonNextTrack>(*this, pin, channelSelection, customSonChannel);
            break;
        case 2:
            logDebugP("Initializing button %d as previous track", buttonNumber);
            button = std::make_shared<ButtonPreviousTrack>(*this, pin, channelSelection, customSonChannel);
            break;
        case 3:
            logDebugP("Initializing button %d as device button", buttonNumber);
            button = std::make_shared<ButtonDevice>(*this, pin, customDevice);
            break;
    }
}

void SonosNFCPlayerModule::handleButtons()
{
    if (_button1 != nullptr)
        _button1->loop();
    if (_button2 != nullptr)
        _button2->loop();
    if (_button3 != nullptr)
        _button3->loop();
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
    auto currentPlayHandle = _currentPlayHandle;
    if (currentPlayHandle != nullptr && currentPlayHandle->isTimedOut())
    {
        if (_playingNotPossibleSince == 0)
        {
            _playingNotPossibleSince = max(2UL, millis());
            currentPlayHandle->channel().pause();
        }
        else if (_playingNotPossibleSince > 1 && millis() - _playingNotPossibleSince >= 5000)
        {
            _playingNotPossibleSince = 1; // Stop report playing not possible after 5 seconds      
        }
    }
    handleLeds();
    handleRotaryControls();
    handleButtons();
    handleTempVolume();
    if (_cardReader == nullptr)
        return;
  
    auto cardReaderState = _cardReader->state();
    if (cardReaderState != _cardReaderState)
    {
        _cardReaderState = cardReaderState;
        logDebugP("Card reader changed: %d", (int)cardReaderState);
    }
    auto currentCard = _cardReaderState == CardReaderState::TagAvailable ? _cardReader->currentCard() : nullptr;
    if (currentCard != _currentCard)
    {
        auto previousCard = _currentCard;
        _currentCard = currentCard;
        _handlingCardInProgress = true;
        if (_currentCard != nullptr)
        {
            _pulsingInterval = _currentCard->hasCommand("SINGLE") ? 667 : 909;
            _currentPlayHandle = nullptr;
            logInfoP("New card detected");
            logIndentUp();
            _currentCard->logInformation();
            logIndentDown();
            if (configured)
            {
                KoPLY_Card.value(true, DPT_Switch);
                KoPLY_CardId.value(_currentCard->getUid().c_str(), DPT_String_ASCII);
            }
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
            if (configured)
            {
                KoPLY_Card.value(false, DPT_Switch);
                KoPLY_CardId.value("", DPT_String_ASCII);
            }
        }
        _handlingCardInProgress = false;
    }
    if (openknxSonosModule.isInitialized() == false)
        return;
    if (!_startupFinishedPlayAllowed && _cardReaderState == CardReaderState::TagAvailable || _cardReaderState == CardReaderState::Idle)
    {
        _startupFinishedPlayAllowed = true;
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
    std::string wifi = "";
    std::string password = "";
    std::string mc = "";

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
        if (_startupFinishedPlayAllowed)
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
                        setMainChannel(_configuredMainChannel);
                        logDebugP("Setting main speaker to configured channel");
                         if (!hasPlayCommand) notifyCommandProcessing();
                    }
                    else if (command.tryGetParameterAsPercent(channelNumber))
                    {
                        auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                        if (channel != nullptr)
                        {
                            setMainChannel(channel);
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
                        setSecondaryChannel(_configuredSecondaryChannel);
                        logDebugP("Setting secondary speaker to configured channel");
                        if (!hasPlayCommand) notifyCommandProcessing();
                    }
                    else if (command.tryGetParameterAsPercent(channelNumber))
                    {
                        if (channelNumber == 0)
                        {
                            setSecondaryChannel(nullptr);
                            logDebugP("Disabling secondary speaker");
                            if (!hasPlayCommand) notifyCommandProcessing();
                        }
                        else
                        {
                            auto channel = openknxSonosModule.getChannel(channelNumber - 1);
                            if (channel != nullptr)
                            {
                                setSecondaryChannel(channel);
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
        else if (name == "progmode")
        {
            knx.progMode(command.getParameterAsBool(true));
        }
        else if (name == "wifi")
        {
            wifi = command.parameter;
        }
        else if (name == "password")
        {
            password = command.parameter;
        }
        else if (name == "mc")
        {
            mc = command.parameter;
        }   
        else if (name == "clearcache")
        {
            _cardReader->clearCardCache();
        }
    }
    if (!uri.empty() && _mainChannel != nullptr)
    {
        _currentPlayHandle = _mainChannel->start(uri.c_str(), title.c_str(), image.c_str(), _filePathPrefix.c_str(), _startupFinishedPlayAllowed);
        logDebugP("Started: %s", _currentPlayHandle.get() == nullptr ? "failed" : _currentPlayHandle.get()->uri());
        if (tempPercentage != 255)
        {
            setTempVolumeGroup(tempPercentage, _currentPlayHandle);
        }
        if (_secondaryChannel != nullptr && _startupFinishedPlayAllowed)
            _secondaryChannel->joinToGroupCoordinatorOf(_mainChannel);
    }
    else if (pause && _startupFinishedPlayAllowed)
    {
        if (_mainChannel != nullptr && ParamPLY_StopOnRemoveTag && _mainChannel->isPlaying(_currentPlayHandle.get()) == TagPlayState::Playing)
        {
            _mainChannel->pause();
        }
    }
    else if (continuePlaying && _startupFinishedPlayAllowed)
    {
        continuePlay();
    }
    bool needRestart = false;
    if (!wifi.empty())
    {
        openknxNetwork.saveWifiSettings(wifi.c_str(), password.c_str(), false);
        needRestart = true;
    }
    if (!mc.empty())
    {
        IPAddress new_address;
        std::string new_address_str = mc;
        if (new_address_str == "") new_address_str = "0.0.0.0";
        new_address.fromString(new_address_str.c_str());
        openknxNetwork.setMulticastAddress(new_address, false);
        needRestart = true;
    }
    if (needRestart)
    {
        openknx.restart();
    }
}

void SonosNFCPlayerModule::handleRotaryControls()
{
    if (_rotaryControl1 != nullptr)
        _rotaryControl1->loop();
    if (_rotaryControl2 != nullptr)
        _rotaryControl2->loop();
    if (_rotaryControl3 != nullptr)
        _rotaryControl3->loop();
}
void SonosNFCPlayerModule::continuePlay()
{
    togglePlay(ChannelSelection::Main, 0, true);
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
        bool isPlaying = _tempVolumePlayHandle == nullptr || _tempVolumePlayHandle->isPlaying() != TagPlayState::Stopped;
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
    if (_rotaryControl1 != nullptr)
        _rotaryControl1->processInputKo(ko);
    if (_rotaryControl2 != nullptr)
        _rotaryControl2->processInputKo(ko);
    if (_rotaryControl3 != nullptr)
        _rotaryControl3->processInputKo(ko);
    
    switch (ko.asap())
    {
        case PLY_KoMainChannel:
        {
            uint8_t value = ko.value(DPT_Value_1_Ucount);
            auto channel = openknxSonosModule.getChannel(value);
            if (channel != nullptr)
            {
                setMainChannel(channel);
            }
        }
        break;
        case PLY_KoSecondaryChannel:
        {
            uint8_t value = ko.value(DPT_Value_1_Ucount);
            if (value == 63)
            {
                setSecondaryChannel(nullptr);
            }
            else
            {
                auto channel = openknxSonosModule.getChannel(value);
                if (channel != nullptr)
                {
                    setSecondaryChannel(channel);
                }
            }
        }
    }
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
           _cardReaderState == CardReaderState::TagAvailable;
}

TagPlayState SonosNFCPlayerModule::isPlayingTag() const
{
    if (_mainChannel == nullptr)
        return TagPlayState::Stopped;
    return _mainChannel->isPlaying(_currentPlayHandle.get());
}

CardReaderState SonosNFCPlayerModule::cardReaderState() const
{
    return _cardReaderState;
}

PlayerState SonosNFCPlayerModule::playerState() const
{
    if (!_startupFinishedPlayAllowed)
    {
        return PlayerState::Startup;
    }
    if (_lastCommandProcessTime != 0)
    {
        return PlayerState::CommandProcessing;
    }
    if (_handlingCardInProgress)
    {
        return PlayerState::CardReaderTagReading;
    }
    if (_settingVolumeInProgress)
    {
        return PlayerState::SettingVolume;
    }
    if (_playingNotPossibleSince > 1)
            return PlayerState::PlayingNotPossible;
    switch (_cardReaderState)
    {
    case CardReaderState::Error:
        return PlayerState::CardReaderError;
    case CardReaderState::Initializing:
        return PlayerState::CardReaderInitializing;
    case CardReaderState::TagReading:
        return PlayerState::CardReaderTagReading;
    case CardReaderState::TagAvailable:
        switch (isPlayingTag())
        {
        case TagPlayState::Playing:
            return PlayerState::PlayingTag;
        case TagPlayState::WaitForResponse:
            return PlayerState::CardReaderTagReading;
        default:
            return PlayerState::CardReaderTagAvailable;
        }
    case CardReaderState::Idle:
        return PlayerState::Idle;
    default:
        logError(const_cast<SonosNFCPlayerModule*>(this)->logPrefix() , "Unknown card reader state: %d", (int)_cardReaderState);
        return PlayerState::Idle;
    }
}

uint16_t SonosNFCPlayerModule::getPulsingInterval() const
{
    return _pulsingInterval;
}

void SonosNFCPlayerModule::setVolumeRelative(ChannelSelection channelSelection, uint8_t customChannel, int8_t diff)
{
    auto channel = getChannel(channelSelection, customChannel);
    if (channel != nullptr)
    {
        _settingVolumeInProgress = true;
        handleLeds();
        channel->setVolumeRelative(diff);
        _settingVolumeInProgress = false;
    }
}

void SonosNFCPlayerModule::setGroupVolumeRelative(ChannelSelection channelSelection, uint8_t customChannel, int8_t diff)
{
    auto channel = getChannel(channelSelection, customChannel);
    if (channel != nullptr)
    {
        _settingVolumeInProgress = true;
        handleLeds();
        channel->setGroupVolumeRelative(diff);
        _settingVolumeInProgress = false;
    }
}

void SonosNFCPlayerModule::togglePlay(ChannelSelection channelSelection, uint8_t customChannel, bool onlyPlay)
{
    auto channel = getChannel(channelSelection, customChannel);
    if (channel != nullptr)
    {
        switch (channel->getPlayState())
        {
        case SonosApiPlayState::Paused_Playback:
            channel->play();
            break;
        case SonosApiPlayState::Stopped:
            if (channel == _mainChannel || channel == _secondaryChannel)
                channel->start(_currentPlayHandle);
            break;
        case SonosApiPlayState::Transitioning:
        case SonosApiPlayState::Playing:
            if (!onlyPlay)
                channel->pause();
            break;
        default:
            break;
        }
    }
}

SonosChannel* SonosNFCPlayerModule::getChannel(ChannelSelection channel, uint8_t customChannel)
{
    switch (channel)
    {
    case ChannelSelection::Main:
        return _mainChannel;
    case ChannelSelection::Secondary:
        return _secondaryChannel;
    case ChannelSelection::Custom:
        if (customChannel == 0)
            return nullptr;
        return openknxSonosModule.getChannel(customChannel - 1);
    default:
        return nullptr;
    }
}

void SonosNFCPlayerModule::nextTrack(ChannelSelection channelSelection, uint8_t customChannel)
{
    auto channel = getChannel(channelSelection, customChannel);
    if (channel != nullptr)
    {
        channel->nextTrack();
    }
}

void SonosNFCPlayerModule::previousTrack(ChannelSelection channelSelection, uint8_t customChannel)
{
    auto channel = getChannel(channelSelection, customChannel);
    if (channel != nullptr)
    {
        channel->previousTrack();
    }
}