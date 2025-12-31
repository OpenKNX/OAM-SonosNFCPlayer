// =========================
// Includes
// =========================

#include <NfcAdapter.h>
#include <string>
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
  PN532_SPI pn532spi(SPI, 10);
  PN532 pn532(pn532spi);
  NfcAdapter nfcAdapter(pn532spi);
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
  PN532_HSU pn532hsu(SerialWrapper);
  PN532 pn532(pn532hsu);
  NfcAdapter nfcAdapter(pn532hsu);

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

  PN532_I2C pn532_i2c(Wire);      // I2C interface
  PN532 pn532(pn532_i2c);
  NfcAdapter nfcAdapter(pn532_i2c);
#define SDA_PIN 16
#define SCL_PIN 17
#define PN532_TAG_READ_TIMEOUT 1
//#define PN532_IRQ 27 // IRQ pin from PN532 (active LOW)
#endif



// =========================
// Pin definitions
// =========================

#define LED_PIN 2    // LED pin to indicate tag presence








// =========================
// Loop variables
// =========================
static bool tagActive = false;          // True while a tag is present
static unsigned long debounceStart = 0; // Debounce timer
const unsigned long debounceTime = 150; // Debounce duration in ms

std::string handleTag(NfcTag &tag);
void nfcTask(void *pvParameters);
// =========================
// Setup
// =========================
void setup()
{
  Serial.begin(115200);
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
  pinMode(LED_PIN, OUTPUT);         // LED indicates tag presence
  digitalWrite(LED_PIN, LOW);       // LED off initially


  Serial.println("Starting PN532");


  nfcAdapter.begin(); // Initialize PN532

// Start NFC Task
  xTaskCreate(
    nfcTask,         // Task function
    "NFC Task",      // Name
    4096,            // Stack size
    NULL,            // Parameter
    1,               // Priority
    NULL             // Task handle
  );

  Serial.println("PN532 ready");
}


// FreeRTOS Task
void nfcTask(void *pvParameters) {
  const unsigned long pollInterval = 50; // ms
  bool tagPresent = false;
  std::string text = "";
  uint8_t lastUid[7] = {0};
  uint8_t lastUidLength = 0;

  int tagReadFailedCount = 0;
  for (;;) {
    uint8_t uid[7];
    uint8_t uidLength;
    bool detected = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength
#ifdef PN532_TAG_READ_TIMEOUT    
    , PN532_TAG_READ_TIMEOUT
#endif
    );

    // Tag detected
    if (detected) {
     
      if (lastUidLength == uidLength && memcmp(lastUid, uid, uidLength) == 0)
      {
        // same tag as before
      }
      else
      {
        NfcTag tag = nfcAdapter.read();
        std::string result = handleTag(tag);

        NfcTag tag2 = nfcAdapter.read();
        std::string result2 = handleTag(tag2);

        if (result != result2)
        {
          Serial.println("Warning: Inconsistent tag reads!");
        }
        else
        {
          lastUidLength = uidLength;
          memset(lastUid, 0, sizeof(lastUid));
          memcpy(lastUid, uid, uidLength);
          digitalWrite(LED_PIN, HIGH);
          Serial.print("UID: ");
          for (unsigned int i = 0; i < uidLength; i++)
          {
            if (uid[i] < 0x10)
            {
              Serial.print("0");
            }
            Serial.print(uid[i], HEX);
            if (i != uidLength - 1)
            {
              Serial.print(" ");
            }
          }
          Serial.println();

          Serial.print("Content: ");
          Serial.println(result.c_str());
          text = result;
          tagPresent = true;
          digitalWrite(LED_PIN, HIGH);
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
        text = "";
        lastUidLength = 0;
        memset(lastUid, 0, sizeof(lastUid));
        tagReadFailedCount = 0;
        digitalWrite(LED_PIN, LOW);
        Serial.println("Tag removed");
      }
      else
      {
        continue; // try again
      }
    }

    vTaskDelay(pdMS_TO_TICKS(pollInterval));
  }
}

unsigned long timeStamp = 0;
unsigned long previousMessage = 0;
// =========================
// Main loop
// =========================
void loop()
{
  auto currentMillis = millis();
  if (currentMillis - timeStamp >= 3) {
    if (previousMessage == 0)
    {
      Serial.printf("loop time: %d ms\r", currentMillis - timeStamp);
      previousMessage = currentMillis;
    }
  }
  if (currentMillis - previousMessage >= 500) {
    previousMessage = 0;
  }
  timeStamp = currentMillis;
  
}

// =========================
// Tag processing function
// =========================
std::string handleTag(NfcTag &tag)
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
      for (int i = 0; i < textLength; i++)
      {
        result += (char)payload[1 + langLength + i];
      }
      delete[] payload;

      

    }
  }
  return result;
}