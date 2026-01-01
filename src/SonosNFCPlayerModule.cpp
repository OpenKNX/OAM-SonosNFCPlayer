#include "SonosNFCPlayerModule.h"

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
}

void SonosNFCPlayerModule::loop(bool configured)
{
    if (_cardReader == nullptr)
        return;
    auto currentCard = _cardReader->currentCard();
    if (currentCard != _currentCard)
    {
        _cardReader->logInformation();
        _currentCard = currentCard;
        if (_currentCard)
        {
            logInfoP("Card with content: %s", _currentCard->getTextContent().c_str());
            KoPLY_Card.value(true, DPT_Switch);
        }
        else
        {
            logInfoP("No card present");
            KoPLY_Card.value(false, DPT_Switch);
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
        if (_currentCard)
        {
            logInfoP("Current card content: %s", _currentCard->getTextContent().c_str());
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

