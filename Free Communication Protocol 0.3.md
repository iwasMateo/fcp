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

Für Response:

1. READY (Wert = 001)

Der Peer hat eine Verbindung aufgenommen und ist bereit Daten zu empfangen.

2. BUSY (Wert = 010)

Der Peer hat eine Verbindung aufgenommen aber kann im Moment keine Daten empfangen. Versuche erneut in 20 Sekunden.

3. BLOCKED (Wert = 011)

Der Peer hat eine Verbindung aufgenommen wird sie aber nach dieser Nachricht wieder schließen und danach keine weiteren Anfragen annehmen bis auf seiner Seite der Block vom Nutzer aufgehoben wird.

4. DATA (Wert = 100)

Antwort auf eine Request mit dem Typ DATA, enthält im Daten Teil der Nachricht das fehlende Paket nach welchem gefragt wurde.

#### Bit 6
Angabe ob ein Erweiterter header vorhanden ist.

#### Bit 7 
Schon Authentifiziert? Ja/Nein

### Extended headers
Der Erweiterte Header erlaubt einem Nutzer weiter nötige Informationen weiterzuleiten. 
Der erweiterte Header besteht aus 10 Bytes.
#### Bytes 0-7: Peer-ID
Die Peer-ID ist eine Version des public keys des Peers. Das erfolgt so:
SHA-256(public key) und davon die ersten 8 Bytes.
Dieses Verfahren stellt noch keine sichere Verbindung oder Authentifizierung her aber gibt zumindenst einmal eine kleine verifikation. 
