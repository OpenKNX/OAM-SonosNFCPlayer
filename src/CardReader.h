#pragma once
#include "Card.h"
#include <NfcAdapter.h>
#include <string>
#include <memory>

enum CardReaderState
{
    CARD_READER_STATE_INITIALIZING,
    CARD_READER_STATE_IDLE,
    CARD_READER_ERROR,
    CARD_READER_STATE_TAG_READING,
    CARD_READER_STATE_AVAILABLE,

};

class CardReader
{
    CardReaderState _state = CARD_READER_STATE_INITIALIZING;
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
    
};