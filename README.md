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

- ESP32 Dev-Board 
- PN532 Board
- 3xKY-040 Drehwinkelgeber
- 1 LED
- Beliebig viele NFC Tags

## NFC Tags

Es können alle Standard NFC Tags verwendet werden.
Anleitung unter [Programmierung der NFC Tags](./doc/Applikationsbeschreibung.md#programmierung-der-nfc-tags) in der Anwendungsdokumentation zu finden.

## Lizenz

Diese Software steht unter der [GNU GPL v3](LICENSE).