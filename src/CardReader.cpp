#include "cardreader.h"
#include "OpenKNX.h"

// #define PN532_RSTPDN 26
#define PN532_RSTPDN 26
#if 0
  #define NFC_INTERFACE_SPI
  #include <SPI.h>
  #include <PN532_SPI.h>
  #include <PN532_SPI.cpp>
  #include "PN532.h"

  #define PN532_CS   5
  #define PN532_MISO 19
  #define PN532_MOSI 23
  #define PN532_SCK  18
  #define PN532_IRQ 27 // IRQ pin from PN532 (active LOW)
  PN532_SPI pn532interface(SPI, 10);
#elif 1
  #define NFC_INTERFACE_HSU
  #include <PN532_HSU.h>
  #include <PN532_HSU.cpp>
  #include <PN532.h>
 
  class HardwareSerialWrapper : public HardwareSerial {
    public:;
    HardwareSerialWrapper(HardwareSerial& serial) : HardwareSerial(serial) {}

    int read() override {
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


#define PN532_RX 16
#define PN532_TX 17
#define PN532_TAG_READ_TIMEOUT 100
 // PN532 nfc(pn532hsu);
#else 
  #define NFC_INTERFACE_I2C
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532_I2C.cpp>
  #include <PN532.h>

  PN532_I2C pn532interface(Wire);      // I2C interface
#define SDA_PIN 16
#define SCL_PIN 17
#define PN532_TAG_READ_TIMEOUT 1
//#define PN532_IRQ 27 // IRQ pin from PN532 (active LOW)
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
#ifdef PN532_IRQ
    pinMode(PN532_IRQ, INPUT_PULLUP); // IRQ is active LOW
#endif
    _pn532.begin();
    _versionData = _pn532.getFirmwareVersion();
    if (_versionData)
    {
        _nfcAdapter.begin();
        xTaskCreate(
            [](void *pvParameters)
            { static_cast<CardReader *>(pvParameters)->nfcTask(); }, // Function to implement the task
            "NFC Task",                                              // Name
            4096,                                                    // Stack size
            this,                                                    // Parameter
            1,                                                       // Priority
            NULL                                                     // Task handle
        );
    }
    logInformation();
}

void CardReader::nfcTask()
{
    const unsigned long pollInterval = 50; // ms
    bool tagPresent = false;
    uint8_t lastUid[7] = {0};
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

            if (lastUidLength == uidLength && memcmp(lastUid, uid, uidLength) == 0)
            {
                // same tag as before
            }
            else
            {
                NfcTag tag = _nfcAdapter.read();
                std::string result = handleTag(tag);

                NfcTag tag2 = _nfcAdapter.read();
                std::string result2 = handleTag(tag2);

                if (result != result2)
                {
                    logWarningP("Inconsistent tag reads detected");
                }
                else
                {
                    lastUidLength = uidLength;
                    memset(lastUid, 0, sizeof(lastUid));
                    memcpy(lastUid, uid, uidLength);
                    tagPresent = true;
                    _currentCard = std::make_shared<Card>(result);
                }
            }
        }
        else if (tagPresent)
        {
            tagReadFailedCount++;
            if (tagReadFailedCount >= 2)
            {
                // reset last tag info after several failed reads
                tagPresent = false;
                lastUidLength = 0;
                _currentCard = nullptr;
            }
            else
            {
                continue; // try again
            }
        }

        vTaskDelay(pdMS_TO_TICKS(pollInterval));
    }
}

std::shared_ptr<Card> CardReader::currentCard()
{
    return _currentCard;
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