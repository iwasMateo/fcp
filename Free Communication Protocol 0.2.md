# Das Ziel
Ein funktionierendes Kommunikationsprotokoll welches weder einen Server noch Vertrauen gegenüber Fremden Peers benötigt.

# Umsetzung
## Aufbau von Anfragen
Jegliche Anfrage ist aus mindestens 2 Teilen aufgebaut:
1. 10 Bytes Metadaten
2. X Bytes Nutzername
3. (optional X Bytes Daten)
### Die ersten 10 Bytes
#### Der erste Byte ist der Type der Anfrage. 
Das können sein:
1. Request (Wert = 1)
Eine Request stellt eine Anfrage gegenüber eines Peers. Das erfolgt meist am Anfang um eine Verbindung aufzubauen oder später um fehlende Packete anzufragen. Eine Request muss zwar einen Nutzernamen beinhalten aber keine extra Daten.
2. Response (Wert = 2)
Response ist eine simple Antwort auf eine Request oder Response. Sie trägt sowie einen Nutzernamen als auch Daten hinter sich.
3. Upgrade (Wert = 3)
Eine Upgrade Anfrage wird benutzt um auf eine andere Version des selben Protokolls zu wechseln oder auf ein ganz anderes Protokoll zu wechseln. Mehr dazu später.

#### Der zweite Byte ist der Subtyp der Anfrage.
Verschiedene Anfragetypen können auch nur bestimmte subtypen enthalten.
Für Request:
1. Status (Wert = 1)
Fragt nach dem Status des anderen Peers um festzustellen ob dieser Nachrichten empfangen kann/wird.
2. Data (Wert = 2)
Fragt nach fehlenden Daten innerhalb eines vorherigen Pakets.

Für Response:
1. READY (Wert = 1)
Der Peer hat eine Verbindung aufgenommen und ist bereit Daten zu empfangen.
2. BUSY (Wert = 2)
Der Peer hat eine Verbindung aufgenommen aber kann im Moment keine Daten empfangen. Versuche erneut in 20 Sekunden.
3. BLOCKED (Wert = 3)
Der Peer hat eine Verbindung aufgenommen wird sie aber nach dieser Nachricht wieder schließen und danach keine weiteren Anfragen annehmen bis auf seiner Seite der Block vom Nutzer aufgehoben wird.
4. DATA (Wert = 4)
Antwort auf eine Request mit dem Typ DATA, enthält im Daten Teil der Nachricht das fehlende Paket nach welchem gefragt wurde.

#### Der dritte Byte ist der Protokolltyp:
1. Chat (Wert = 1)
Der Chat Typ nutzt nach dieser Anfrage den gleichen Aufbau wie die initiale Anfrage und nutzt TCP.
2. Audio (Wert = 2)
Der Audiotyp nutzt UDP und sendet nach dieser Anfrage nur Audiodaten bis auf TCP vom gleichen Peer mit dem Audioanruf besteht eine RESPONSE BUSY Anfrage über TCP kommt.
3. File (Wert = 3)
Weiß noch nicht wie ich das machen will. Fürs erste ignorieren.

#### Der vierte und fünfte Byte sind Version und Subversion.
Beide dieser Werte sind einfach nur 1 Byte große Integer welche die Version des Protokolltyps angeben. 

#### Der sexte Byte enthält die Länge des Nutzernamens.
Auch ein einfacher 1 Byte langer Integer welcher die Menge an Bytes des Nutzernamens über UTF-8 angibt.

#### Der siebte bis 10. Byte enthalten die Länge der Daten/Payload.
Dies ist ein normaler 4 Byte Integer welcher die Länge der Payload in Bytes angibt. 

### Die restlichen Daten
1. Der Nutzername
Der Nutzername ist ein Null terminierter String dessen länge in Byte 6 angegeben wurde.
2. Die Payload
Die Payload enthält jegliche weiteren Daten. Das heißt Text für das Chat Protokoll oder fehlende Daten eines Packets.

## Upgrade Anfragen
Upgrade Anfragen sind besonders interessant. Sie erlauben auf einen anderen Protokolltyp zu wechseln selbst wenn nur einer der beiden Peers diesen Typ bisher hatten. Das erfolgt so:
((Konversation über Chat 1.0))
Peer A: "Upgrade Status Peer-A, Video 1.3"
Dann entweder:
Peer B: "Response Ready Peer-B, Video 1.3" - Wonach direkt auf dem Protokoll kommuniziert werden kann.
oder
Peer B: "Upgrade Ready Peer-B, Video 1.3" - Hiernach wird über den FILE Protokolltyp die Erweiterung über TCP übertragen und dann auf Anfrage des Nutzers ausgeführt worauf von Peer B noch einmal   "Response Ready Peer-B, Video 1.3" benötigt wäre
oder
Peer B: "Upgrade Blocked Peer-A, Chat 1.0" - Was wieder zurück zu Chat 1.0 führt (oder das Protokoll welches davor benutzt wurde)