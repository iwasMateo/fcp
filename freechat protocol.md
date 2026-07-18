# Das Ziel
Ein freies open source Peer to Peer Chatprotokoll dass einem Erlaubt Nachrichten zu schicken wenn beide online sind und bis da hin die nachricht speichert und alle 10 minuten versucht die nachricht erneut zu schicken.
# Umsetzung

## Request handling

1. TCP request: "Request Status sender-username, Chat 1.0" directly to receiver IP
2. Wait for response

From here there are 4 types of responses and 3 protocols: 
1. "Response Ready receiver-username, Text 1.0"
Which would tell the sender they can now send packages

2. "Response Busy sender-username, Text 1.0" 
Which would warrant a retry in 10 seconds

3. "Response Blocked sender-username, Text 1.0" 
Which tells the sender that they are blocked

If no response arrives the sender knows that the receiver is not online. In this case they will retry in 5 minutes if the program is still open.

## Protocols
The three protocols are:
1. Chat
Text sent in the markdown format

2. Audio
An Audiostream using UDP

3. File
A TCP protocol that sends files up to a user limited size.
This one requires a second request after a "Ready" response. File size negotiation. That looks like this: "Request Size 1000, File 1.0" with another response which looks like: "Response Ready receiver-name, File 1.0" or "Response False [user defined limit], File 1.0"

## Data sending
2 types:
1. TCP 
This is for File and Chat so there are always complete files at the end of transfer
2. UDP 
Lossy but fine for Audiostreams/Calls

# Full overview

1. Request
2. Response
3. 
a. Potential re-request
b. File contents
