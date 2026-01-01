#pragma once
#include "Card.h"
#include <NfcAdapter.h>
#include <string>
#include <memory>


class CardReader
{
    uint32_t _versionData;
    PN532 _pn532;
    NfcAdapter _nfcAdapter;
    std::shared_ptr<Card> _currentCard;

    std::string logPrefix() const;
    void nfcTask();
    std::string handleTag(NfcTag& tag);
    
public:
    CardReader();
    void setup();
    void logInformation() const;

    std::shared_ptr<Card> currentCard();
    
};