#if defined(KNX_IP_WIFI) || defined(KNX_IP_LAN)
#pragma once
#include "OpenKNX.h"
#include "CardReader.h"

class SonosNFCPlayerModule : public OpenKNX::Module
{
    CardReader* _cardReader;
    std::shared_ptr<Card> _currentCard = nullptr;

  public:
    const std::string logPrefix() override;
    const std::string name() override;
    const std::string version() override;
    void init() override;
    void loop(bool configured) override;
    void setup(bool configured) override;
    bool processCommand(const std::string cmd, bool debugKo);
    void showHelp() override;

};

extern SonosNFCPlayerModule openknxSonosNFCPlayer;
#endif