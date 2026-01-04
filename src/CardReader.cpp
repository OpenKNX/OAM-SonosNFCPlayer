#include "cardreader.h"
#include "OpenKNX.h"

#ifdef NFC_INTERFACE_SPI
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"

#ifndef PN532_CS
#error "PN532_CS not defined for SPI interface"
#endif
#ifndef PN532_MISO
#error "PN532_MISO not defined for SPI interface"
#endif
#ifndef PN532_MOSI
#error "PN532_MOSI not defined for SPI interface"
#endif
#ifndef PN532_SCK
#error "PN532_SCK not defined for SPI interface"
#endif

PN532_SPI pn532interface(SPI, 10);

#elifdef NFC_INTERFACE_HSU
#include <PN532_HSU.h>
#include <PN532.h>

#ifndef PN532_RX
#error "PN532_RX not defined for HSU interface"
#endif
#ifndef PN532_TX
#error "PN532_TX not defined for HSU interface"
#endif
#ifndef PN532_TAG_READ_TIMEOUT
#define PN532_TAG_READ_TIMEOUT 100
#endif

class HardwareSerialWrapper : public HardwareSerial
{
public:
    ;
    HardwareSerialWrapper(HardwareSerial &serial) : HardwareSerial(serial) {}
    int read() override
    {
        int result = HardwareSerial::read();
        if (result < 0)
        {
            delay(1); // give time to other tasks
        }
        return result;
    }
};
HardwareSerialWrapper SerialWrapper(Serial2);
PN532_HSU pn532interface(SerialWrapper);

#elifdef NFC_INTERFACE_I2C
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#ifndef PN532_SDA
#error "PN532_SDA not defined for I2C interface"
#endif
#ifndef PN532_SCL
#error "PN532_SCL not defined for I2C interface"
#endif

#ifndef PN532_TAG_READ_TIMEOUT
#define PN532_TAG_READ_TIMEOUT 1
#endif

PN532_I2C pn532interface(Wire); // I2C interface

#else
#error "No NFC interface defined. Please define one of NFC_INTERFACE_SPI, NFC_INTERFACE_HSU, NFC_INTERFACE_I2C."
#endif

CardReader::CardReader()
    : _pn532(pn532interface), _nfcAdapter(pn532interface)
{
}

std::string CardReader::logPrefix() const
{
    return "CardReader";
}

void CardReader::logInformation() const
{
#ifdef NFC_INTERFACE_I2C
    logInfoP("Using I2C interface");
    logInfoP("SDA: GPIO%d, SCL: GPIO%d", SDA_PIN, SCL_PIN);
#endif
#ifdef NFC_INTERFACE_SPI
    logInfoP("Using SPI interface");
    logInfoP("CS: GPIO%d, MISO: GPIO%d, MOSI: GPIO%d, SCK: GPIO%d", PN532_CS, PN532_MISO, PN532_MOSI, PN532_SCK);
#endif
#ifdef NFC_INTERFACE_HSU
    logInfoP("Using Serial (UART) interface");
    logInfoP("RX: GPIO%d, TX: GPIO%d", PN532_RX, PN532_TX);
#endif
#ifdef PN532_RSTPDN
    logInfoP("RSTPDN: GPIO%d", PN532_RSTPDN);
#else
    logInfoP("RSTPDN not used");
#endif
    if (_versionData == 0)
    {
        logErrorP("Didn't find PN532 board");
    }
    else
    {
        logInfoP("Found chip PN5%02X", (int)(_versionData >> 24) & 0xFF);
        logInfoP("Firmware ver. %d.%d", (int)((_versionData >> 16) & 0xFF), (int)((_versionData >> 8) & 0xFF));
    }
}

void CardReader::setup()
{
#ifdef PN532_RSTPDN
    // Hard reset PN532 (ELECHOUSE requires this)
    pinMode(PN532_RSTPDN, OUTPUT);
    digitalWrite(PN532_RSTPDN, LOW);
    delay(50);
    digitalWrite(PN532_RSTPDN, HIGH);
    delay(200);
#endif

#ifdef NFC_INTERFACE_I2C
    Wire.begin(SDA_PIN, SCL_PIN, 25000); // I2C init

#endif
#ifdef NFC_INTERFACE_SPI
    SPI.begin(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_CS); // SPI init
#endif
#ifdef NFC_INTERFACE_HSU
    // Initialize UART2 for PN532
    Serial2.begin(115200, SERIAL_8N1, PN532_RX, PN532_TX);
#endif

    _pn532.begin();
    _versionData = _pn532.getFirmwareVersion();
    if (_versionData)
    {
        _nfcAdapter.begin();
        xTaskCreatePinnedToCore(
            [](void *pvParameters)
            { static_cast<CardReader *>(pvParameters)->nfcTask(); }, // Function to implement the task
            "NFC Task",                                              // Name
            4096,                                                    // Stack size
            this,                                                    // Parameter
            1,                                                       // Priority
            NULL,                                                    // Task handle
            xPortGetCoreID() ? 0 : 1                                 // Core ID
        );
    }
    else
    {
        _state = CardReaderState::CARD_READER_ERROR;
    }
    logInformation();
}

void CardReader::nfcTask()
{
    _state = CardReaderState::CARD_READER_STATE_IDLE;
    const unsigned long pollInterval = 50; // ms
    bool tagPresent = false;
    uint8_t *lastUid[7] = {0};
    uint8_t lastUidLength = 0;
    int tagReadFailedCount = 0;
    for (;;)
    {
        uint8_t uid[7];
        uint8_t uidLength;
        bool detected = _pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength
#ifdef PN532_TAG_READ_TIMEOUT
                                                   ,
                                                   PN532_TAG_READ_TIMEOUT
#endif
        );

        // Tag detected
        if (detected)
        {
            if (tagReadFailedCount > 0)
            {
                logErrorP("Tag read succeeded after %d failed attempts", tagReadFailedCount);
                tagReadFailedCount = 0;
            }
            if (lastUidLength == uidLength && memcmp(lastUid, uid, uidLength) == 0)
            {
                // same tag as before
            }
            else
            {
                tagPresent = true;     
                _state = CardReaderState::CARD_READER_STATE_TAG_READING;
                NfcTag tag = _nfcAdapter.read();
                std::string result = handleTag(tag);

                NfcTag tag2 = _nfcAdapter.read();
                std::string result2 = handleTag(tag2);

                if (result != result2)
                {
                    logWarningP("Inconsistent tag reads detected");
                    vTaskDelay(pdMS_TO_TICKS(pollInterval));
                    continue; // try again
                }
                else
                {
                    lastUidLength = uidLength;
                    memset(lastUid, 0, sizeof(lastUid));
                    memcpy(lastUid, uid, uidLength);
                    _currentCard = std::make_shared<Card>(uid, uidLength, result.c_str(), result.length());
                    _state = CardReaderState::CARD_READER_STATE_AVAILABLE;
                }
            }
        }
        else if (tagPresent)
        {

            tagReadFailedCount++;
            if (tagReadFailedCount >= 5)
            {
                // reset last tag info after several failed reads
                tagPresent = false;
                tagReadFailedCount = 0;
                lastUidLength = 0;
                _state = CardReaderState::CARD_READER_STATE_IDLE;
                _currentCard = nullptr;
            }
            else
            {
                continue; // try again
            }
        }
        if (_state == CardReaderState::CARD_READER_STATE_INITIALIZING)
        {
            _state = CardReaderState::CARD_READER_STATE_IDLE;
        }

        vTaskDelay(pdMS_TO_TICKS(pollInterval));
    }
}

std::shared_ptr<Card> CardReader::currentCard()
{
    return _currentCard;
}

CardReaderState CardReader::state()
{
    return _state;
}

std::string CardReader::handleTag(NfcTag &tag)
{
    std::string result = "";
    if (!tag.hasNdefMessage())
    {
        return result;
    }

    NdefMessage msg = tag.getNdefMessage();
    for (int i = 0; i < msg.getRecordCount(); i++)
    {
        NdefRecord r = msg.getRecord(i);

        // Check for well-known Text record
        if (r.getTnf() == TNF_WELL_KNOWN &&
            r.getTypeLength() == 1 &&
            r.getType()[0] == 'T')
        {

            // Get payload as byte array
            int payloadLength = r.getPayloadLength();
            byte *payload = new byte[payloadLength];
            r.getPayload(payload);

            // First byte = status (encoding + lang length)
            byte status = payload[0];
            int langLength = status & 0x3F; // lower 6 bits
            // Text starts after status + language code
            int textLength = payloadLength - 1 - langLength;

            result = std::string(textLength, ' ');
            memcpy((char *)result.c_str(), &payload[1 + langLength], textLength);
            delete[] payload;
        }
    }
    return result;
}