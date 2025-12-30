// =========================
// Includes
// =========================

#include <NfcAdapter.h>

#define NFC_INTERFACE_HSU
#include <PN532_HSU.h>
#include <PN532_HSU.cpp>
#include <PN532.h>

      
  PN532_HSU pn532hsu(Serial1);
  PN532 nfc(pn532hsu);

// =========================
// Pin definitions
// =========================
#define PN532_IRQ 27 // IRQ pin from PN532 (active LOW)
#define LED_PIN 2    // LED pin to indicate tag presence


#define PN532_RX 16
#define PN532_TX 17



// =========================
// PN532 setup
// =========================

PN532_HSU pn532_hsu(Serial2);
NfcAdapter nfcAdapter(pn532_hsu);

// =========================
// Loop variables
// =========================
static bool tagActive = false;          // True while a tag is present
static unsigned long debounceStart = 0; // Debounce timer
const unsigned long debounceTime = 150; // Debounce duration in ms

void handleTag(NfcTag &tag);

// =========================
// Setup
// =========================
void setup()
{
  Serial.begin(115200);

 // Initialize UART2 for PN532
  Serial2.begin(115200, SERIAL_8N1, PN532_RX, PN532_TX);


  pinMode(PN532_IRQ, INPUT_PULLUP); // IRQ is active LOW
  pinMode(LED_PIN, OUTPUT);         // LED indicates tag presence
  digitalWrite(LED_PIN, LOW);       // LED off initially

  nfcAdapter.begin(); // Initialize PN532

  Serial.println("PN532 + LED ready");
}
unsigned long timeStamp = 0;
// =========================
// Main loop
// =========================
void loop()
{

  auto now = millis();
  bool irqLow = //(digitalRead(PN532_IRQ) == LOW); // Check if a tag is present
nfcAdapter.tagPresent();
  auto need = millis() - now;
  if (now - timeStamp > 1000)
  {
    timeStamp = now;
    Serial.println("Loop time (ms): " + String(need));
  }

  // --- Tag detected (after debounce) ---
  if (irqLow && !tagActive)
  {
    if (debounceStart == 0)
    {
      debounceStart = millis(); // Start debounce timer
    }

    if (millis() - debounceStart >= debounceTime)
    {
      tagActive = true;
      debounceStart = 0;

      digitalWrite(LED_PIN, HIGH); // LED on while tag is present

      NfcTag tag = nfcAdapter.read(); // Read tag once
      handleTag(tag);
    }
  }

  // --- Tag removed ---
  if (!irqLow)
  {
    debounceStart = 0;

    if (tagActive)
    {
      tagActive = false;
      digitalWrite(LED_PIN, LOW); // LED off when tag removed
    }
  }
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

      Serial.print("Text: ");
      Serial.println(text);
    }
  }
}