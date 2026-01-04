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

Der Player spielt Quellen deren Referenz in einem Standard NFC Tag hinterlegt ist auf dem Sonos System ab.

Die NFC Tags müssen mit einem NFC-NDEF-Record des Types 'Text' programmiert werden.

### Programmierung der NFC Tags

Die NFC Tags werden am Besten mit einem Mobiltelefon beschrieben.

Beispielsweise kann die App 'NFC Tools' dazu verwendet werden. 
<!-- DOC Skip="1"-->
[Apple Store](https://apps.apple.com/at/app/nfc-tools/id1252962749) [Google Play Store](https://play.google.com/store/apps/details?id=com.wakdev.wdnfc&hl=de_AT) dazu verwendet werden.

Es muss ein Datensatz vom Type "Text" hingzugefügt werden. Der Text besteht aus bis zu vier Zeilen.

- Zeile 1: URI
  Gibt die URI der Resource an, die bespielt werden soll. Ist die Zeile leer, wird nichts gespielt
- Zeile 2: Zusätzliche Befehle
  Optional: Gibt zusätzliche Befehle an den Player. 
- Zeile 3: Titel
  Optional: Gibt den Titel der angzeigt werden soll an. 
  Wird nur für Radio-Streams verwendet.
- Zeile 4: Bild-URL
  Optional: Gibt eine URL an, die als Bild verwendet werden soll.
  Wird nur für Radio-Streams verwendet.

Achtung: in der 'NFC Tools' Apps, werden zu lange Zeilen auch automatisch umgebrochen. 
Diese automatischen Umbrüche zählen aber nicht als eigene Zeile. Eine neue Zeile wird nur über die ↵ Taste erzeugt. 
Am Besten stellt man die Texte zuerst in einen anderen Editor zusammen und kopiert sie anschließend in die App.

#### Aufbau URI

##### Radio
x-rincon-mp3radio://&lt;Streaming URL&gt;

Beispiel: x-rincon-mp3radio://https://orf-live.ors-shoutcast.at/wie-q2a
Bei Verwendung von Radio Streams, muss auch die Zeile 3 und 4 verwendet werden.

<!-- DOC Skip="7"-->
Vollständiges Beispiel:
```
x-rincon-mp3radio://https://orf-live.ors-shoutcast.at/wie-q2a

ORF Radio Wien
https://cdn-profiles.tunein.com/s44255/images/logod.jpg
```

#### Sonos Playlist
x-playlist:&lt;Name der Playlist&gt;

Beispiel: playlist:Meine besten Lieder

#### Mediathek
x-playlist:&lt;Relativer Pfad in der Mediathek&gt;

Der Pfad wird mit dem Einstellung "Dateifreigabe Prefix" aus der ETS konfiguration ergänzt. 
Er wird deshalb nur relativ dazu angegeben.
Als Pfadseparatoren müssen '/' verwendet werden (nicht '\'). 
Soll ein Ordner gespielt werden, muss der Pfad mit '/' enden. 
Es können nur Ordner abgespielt werden, die zuvor in der Sonos App über die Synchronisation zur Mediathek hinzugefügt wurden.

Beispiel: x-file-cifs:Pink Floyd/The Wall/

Dateifreigabe Prefix: //192.168.0.1/Share/Storage/Musik
Abgespielt wird: //192.168.0.1/Share/Storage/Musik/Pink Floyd/The Wall/


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
