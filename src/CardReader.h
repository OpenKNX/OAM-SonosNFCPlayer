#pragma once
#include "Card.h"
#include <NfcAdapter.h>
#include <string>
#include <memory>

enum class CardReaderState
{
    Initializing = 0,
    Idle,
    Error,
    TagReading,
    TagAvailable,
    CardNotPlayable
};

class CardReader
{
    std::vector<std::shared_ptr<Card>> _cardCash = std::vector<std::shared_ptr<Card>>();
    volatile CardReaderState _state = CardReaderState::Initializing;
    uint32_t _versionData = 0;
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
    CardReaderState state();
    void clearCardCache();
    
};