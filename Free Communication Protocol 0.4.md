# Das Ziel

Ein funktionierendes Kommunikationsprotokoll welches weder einen Server noch Vertrauen gegenüber Fremden Peers benötigt.

# Umsetzung

## Aufbau

### Der erste Byte
#### Bits 0-2
Die ersten 3 Bits Halten den Typ der Anfrage.
Das können sein:

1. REQUEST (Wert = 001)

Eine Request stellt eine Anfrage gegenüber eines Peers. Das erfolgt meist am Anfang um eine Verbindung aufzubauen oder später um fehlende Packete anzufragen. Eine Request muss zwar einen Nutzernamen beinhalten aber keine extra Daten.

2. RESPONSE (Wert = 010)

Response ist eine simple Antwort auf eine Request oder Response. Sie trägt sowie einen Nutzernamen als auch Daten hinter sich.

3. UPGRADE (Wert = 011)

Eine Upgrade Anfrage wird benutzt um auf eine andere Version des selben Protokolls zu wechseln oder auf ein ganz anderes Protokoll zu wechseln. Mehr dazu später.

#### Bits 3-5
Verschiedene Anfragetypen können auch nur bestimmte subtypen enthalten.

Für Request:

1. STATUS (Wert = 001)

Fragt nach dem Status des anderen Peers um festzustellen ob dieser Nachrichten empfangen kann/wird.

2. DATA (Wert = 010)

Fragt nach fehlenden Daten innerhalb eines vorherigen Pakets.

3. AUTHENTICATE (Wert = 011)

Stellt eine Anfrage nach Authentifizierung/Reauthentifizierung. Hierzu später mehr. 

4. EXIT (Wert = 100)
Eine einfache Anfrage den Chat zu beenden.

Für Response:

1. READY (Wert = 001)

Der Peer hat eine Verbindung aufgenommen und ist bereit Daten zu empfangen.

2. BUSY (Wert = 010)

Der Peer hat eine Verbindung aufgenommen aber kann im Moment keine Daten empfangen. Versuche erneut in der Zeit die in der Payload angegeben wird (Format ist uint32).

3. BLOCKED (Wert = 011)

Der Peer hat eine Verbindung aufgenommen wird sie aber nach dieser Nachricht wieder schließen und danach keine weiteren Anfragen annehmen bis auf seiner Seite der Block vom Nutzer aufgehoben wird.

4. DATA (Wert = 100)

Antwort auf eine Request mit dem Typ DATA, enthält im Daten Teil der Nachricht das fehlende Paket nach welchem gefragt wurde.

5. ERROR (Wert = 101)
Falls es jegliche Fehler der Formatierung vom Header oder extended header gibt, kann man mit Error eine Anfrage die letzte Nachricht zu wiederholen ausstellen.

#### Bit 6
Angabe ob ein Erweiterter header vorhanden ist.

#### Bit 7 
Schon Authentifiziert? Ja/Nein

### Der Zweite bis Fünfte Byte
Der Zweite bis Fünfte Byte beinhalten die größe der Payload als unsigned int beziehungsweise einem 4 Byte großem unsignierten Integer.

### Extended headers
Der Erweiterte Header erlaubt einem Nutzer weiter nötige Informationen weiterzuleiten. 
Der erweiterte Header besteht aus 4 Bytes eines unsignierten Integers, welcher angibt wie groß der erweiterte Header ist und 32 Bytes des Typs string (nicht null terminiert) um den benutzten standard anzugeben. 
Danach folgen weitere Daten innerhalb der Payload.

### Die Payload
In diesem Teil werden jegliche Daten die nicht direkt zum Header oder Extended Header gehören versendet.
Falls kein Erweiterter Header vorhanden ist, ist das Standard-Format der Payload ein null-terminierter String.