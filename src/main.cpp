// =========================
// Includes
// =========================

#include <NfcAdapter.h>
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
  NfcAdapter nfcAdapter(pn532spi);
#elif 0
  #define NFC_INTERFACE_HSU
  #include <PN532_HSU.h>
  #include <PN532_HSU.cpp>
  #include <PN532.h>
      
  PN532_HSU pn532hsu(Serial2);
  NfcAdapter nfcAdapter(pn532hsu);

#define PN532_RX 16
#define PN532_TX 17
 // PN532 nfc(pn532hsu);
#else 
  #define NFC_INTERFACE_I2C
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532_I2C.cpp>
  #include <PN532.h>

  PN532_I2C pn532_i2c(Wire);      // I2C interface
  NfcAdapter nfcAdapter(pn532_i2c);
#define SDA_PIN 16
#define SCL_PIN 17
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

void handleTag(NfcTag &tag);
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
  bool lastTagState = false;
  const unsigned long pollInterval = 100; // ms
  bool tagPresent;
  for (;;) {
    bool detected = nfcAdapter.tagPresent(1);

    // Tag detected
    if (detected && !lastTagState) {
      lastTagState = true;
      tagPresent = true;
      digitalWrite(LED_PIN, HIGH);

      NfcTag tag = nfcAdapter.read();
      handleTag(tag);
    }

    // Tag removed
    if (!detected && lastTagState) {
      lastTagState = false;
      tagPresent = false;
      digitalWrite(LED_PIN, LOW);
      Serial.println("Tag removed");
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
void handleTag(NfcTag &tag)
{
  Serial.println("NFC tag detected");

  Serial.print("UID: ");
  Serial.println(tag.getUidString());

  if (!tag.hasNdefMessage())
  {
    Serial.println("No NDEF message found");
    return;
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
      String text = "";
      for (int i = 0; i < textLength; i++)
      {
        text += (char)payload[1 + langLength + i];
      }
      delete[] payload;

      if (nfcAdapter.tagPresent(1))
      {
        Serial.print("Text: ");
        Serial.println(text);
      }
      else
      {
        Serial.println("Read failed, tag removed too quickly");
      }

    }
  }
}