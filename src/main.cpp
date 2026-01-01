// =========================
// Includes
// =========================

#include "CardReader.h"



// =========================
// Pin definitions
// =========================

#define LED_PIN 2    // LED pin to indicate tag presence


CardReader* cardReader = nullptr;

// =========================
// Setup
// =========================
void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);         // LED indicates tag presence
  digitalWrite(LED_PIN, LOW);       // LED off initially
  cardReader = new CardReader();

  Serial.println("Starting PN532");
  cardReader->setup();
  


}


std::shared_ptr<Card> _currentCard = nullptr;

// =========================
// Main loop
// =========================
void loop()
{
  auto currentCard = cardReader->currentCard();
  if (currentCard !=  _currentCard)
  {
    _currentCard = currentCard;
    if (_currentCard)
    {
      Serial.print("Card detected with content: ");
      Serial.println(_currentCard->getTextContent().c_str());
    }
    else
    {
      Serial.println("Card removed");
    }
  }
}
