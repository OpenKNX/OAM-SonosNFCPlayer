# OAM-SonosNFCPlayer

Diese SW implementiert einen virtueller Plattenspieler für Sonos-Lautsprecher.
Als Platten werden NFC-Tags zur Auswahl der Sound-Quelle verwendet.

## Features

- Konfigurierbar über KNX-IP 
- Erlaubt die Auswahl von Sound-Quellen mithilfe von NFC-Tags.
- Steuerung der Lautstärke von 2 Einzellautsprechern und der Gruppe über Drehwinkel-Geber.

## Anwenderdokumentation

Die Anwenderdokumentation ist [hier](./doc/Applikationsbeschreibung.md) zu finden.

## Firmware

Eine vorkomplierte Firmware ist [hier](https://github.com/OAM-SonosNFCPlayer/releases) zu finden. ZIP Datei herunterladen, entpacken und der Anleitung im Readme folgen.

## Hardware

- ESP32 Dev-Board (z.B. ESP32 DevKitC v4)
- PN532 Board
- 3 x KY-040 Drehwinkelgeber
- 1 LED (rot) für Progmodus
- 1 LED für Player Status (oder je nach Konfiguration in der ETS)
- 1 LED (optional) für NFC-TAG Status (oder je nach Konfiguration in der ETS) 
- 1 LED (optional) beliebig verwendbar (je nach Konfiguration in der ETS)
- pro LED 1x 220 Ohm Wiederstand 
- Beliebig viele NFC Tags

Um die vorkomplierte Firmware zu nutzen, muss folgende Pin Belegung verwenden:

| ESP32 GPIO Pin | Beschreibung |
|----------------|--------------| 
| 0              | Prog-Button, die meisten Dev-Boards haben den EN-Button auf diesen Pin, der somit direkt verwenden kann.
| 2              | Prog-LED über Wiederstand mit der Anode (+Pol) verbinden, Kathode (-Pol) auf GND
| 13             | Info LED 1 - Player Status über Wiederstand mit der Anode (+Pol) verbinden, Kathode (-Pol) auf GND
| 12             | Info LED 2 - NFC-Tag über Wiederstand mit der Anode (+Pol) verbinden, Kathode (-Pol) auf GND
| 27             | Info LED 3 - über Wiederstand mit der Anode (+Pol) verbinden, Kathode (-Pol) auf GND
| 32             | Drehwinkelgeber 1 CLK
| 33             | Drehwinkelgeber 1 DT
| 21             | Drehwinkelgeber 1 Button
| 34             | Drehwinkelgeber 2 CLK
| 35             | Drehwinkelgeber 2 DT
| 22             | Drehwinkelgeber 2 Button
| 26             | Drehwinkelgeber 3 CLK
| 25             | Drehwinkelgeber 3 DT
| 17             | Drehwinkelgeber 3 Button
| 18             | PN532 auf HSU (Uart) konfiguriert RX
| 19             | PN532 auf HSU (Uart) konfiguriert RX

## NFC Tags

Es können alle Standard NFC Tags verwendet werden.
Anleitung unter [Programmierung der NFC Tags](./doc/Applikationsbeschreibung.md#programmierung-der-nfc-tags) in der Anwendungsdokumentation zu finden.

## Lizenz

Diese Software steht unter der [GNU GPL v3](LICENSE).