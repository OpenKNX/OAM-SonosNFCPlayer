# Applikationsbeschreibung Smart Home Bridge

## Wichtige Hinweise

* Diese KNXprod wird nicht von der KNX Association offiziell unterstützt!
* Die Erzeugung der KNXprod geschieht auf Eure eigene Verantwortung!

## Module

Die Sonos App besteht aus folgenden Modulen:

- [Basiseinstellungen](https://github.com/OpenKNX/OGM-Common/blob/v1/doc/Applikationsbeschreibung-Common.md)
- [Netzwerk](https://github.com/OpenKNX/OFM-Network/blob/v1/doc/Applikationsbeschreibung-Netzwerk.md)
- [Sonos](https://github.com/OpenKNX/OFM-Sonos/blob/v1/doc/Applikationsbeschreibung-Sonos.md)
- [Player](#player)
- [Logik](https://github.com/OpenKNX/OFM-LogicModule/blob/v1/doc/Applikationsbeschreibung-Logik.md)
- [Funktionsblöcke](https://github.com/OpenKNX/OFM-FunctionBlocks/blob/v1/doc/Applikationsbeschreibung-FunctionBlocks.md)



<!-- DOC -->
## Player

Der Player spielt Quellen deren Referenz in einem NFC Tag hinterlegt ist auf dem Sonos System ab.

Die NFC Tags müssen mit einem NFC NDEF Record des Types 'Text' programmiert werden.

Die erste Zeile des Textes enthält die Referenz auf die Tonquelle.

Z.B.:
x-rincon-mp3radio://https://orf-live.ors-shoutcast.at/wie-q2a

<!-- DOC -->
### Haupt-Sonoskanal

Sonsos Kanal der vom Player benutzt wird

<!-- DOC -->
### Zweit-Sonoskanal (0-deaktiviert)

Zweiter Kanal der automatisch beim starten zum Hauptkanal gruppiert wird.
0 bedeutet, dass kein weitere Kanal automatisch zum Hauptkanal gruppiert wird.

<!-- DOC -->
### Stoppt Wiedergabe beim Entfernen der Karte

Beim Entfernen der Karte wird die Wiedergabe automatisch gestoppt.

<!-- DOC -->
### Dateifreigabe Präfix

Prefix, der für Dateiwiedergaben hinzugefügt werden soll.

Beispiel:

- Benötigte Referenz: x-file-cifs://192.168.0.1/Share/Storage/Musik/Queen/A Kind of Magic/
- Hinterlegt in 'Dateifreigabe Präfix': //192.168.0.1/Share/Storage/Musik
- Referenz im NFC Tag: x-file-cifs:Queen/A Kind of Magic/
