### Player

Der Player spielt Quellen deren Referenz in einem Standard NFC Tag hinterlegt ist auf dem Sonos System ab.

Die NFC Tags müssen mit einem NFC-NDEF-Record des Types 'Text' programmiert werden.

### Programmierung der NFC Tags

Die NFC Tags werden am Besten mit einem Mobiltelefon beschrieben.

Beispielsweise kann die App 'NFC Tools' dazu verwendet werden. 

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


