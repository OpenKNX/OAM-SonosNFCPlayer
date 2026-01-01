#pragma once
#include "Card.h"
#include <NfcAdapter.h>
#include <string>
#include <memory>


class CardReader
{
    void nfcTask();
    std::string handleTag(NfcTag& tag);
private:
    PN532 _pn532;
    NfcAdapter _nfcAdapter;
    std::shared_ptr<Card> _currentCard;
public:
    CardReader();
    void setup();

    std::shared_ptr<Card> currentCard();
    
};