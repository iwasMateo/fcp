# The Goal

A functioning communication protocol that requires neither a server nor trust towards foreign peers.
# Implementation

## Structure

### The First Byte

#### Bits 0-2
The first 3 bits hold the type of request.
These can be:

1. REQUEST (Value = 001)
A request submits a query to a peer. This usually occurs at the beginning to establish a connection or later to request missing packets. A request must contain a username but no extra data.

2. RESPONSE (Value = 010)
A response is a simple answer to a request or response. It carries both a username and data.

3. UPGRADE (Value = 011)
An upgrade request is used to switch to another version of the same protocol or to a completely different protocol. More on that later.

#### Bits 3-5
Different request types can also contain only certain subtypes.

For Request:

1. STATUS (Value = 001)
Asks for the status of the other peer to determine whether this peer can/will receive messages.

2. DATA (Value = 010)
Asks for missing data within a previous packet.

3. AUTHENTICATE (Value = 011)
Submits a request for authentication/re-authentication. More on this later.

4. EXIT (Value = 100)
A simple request to end the chat.

For Response:

1. READY (Value = 001)
The peer has established a connection and is ready to receive data.

2. BUSY (Value = 010)
The peer has established a connection but cannot currently receive data. Try again at the time specified in the payload (format is uint32).

3. BLOCKED (Value = 011)
The peer has established a connection but will close it after this message and will not accept any further requests until the block is lifted on its side by the user.

4. DATA (Value = 100)
Response to a request of type DATA; contains in the data part of the message the missing packet that was requested.

5. ERROR (Value = 101)
If there are any formatting errors in the header or extended header, one can issue an error request to repeat the last message.

#### Bit 6
Indication of whether an extended header is present.

#### Bit 7
Already authenticated? Yes/No

### The Second to Fifth Byte
The second to fifth byte contain the size of the payload as an unsigned int, that is, a 4-byte unsigned integer.

### Extended Headers
The extended header allows a user to forward further necessary information.
The extended header consists of 4 bytes of an unsigned integer, which specifies the size of the extended header, and 32 bytes of type string (not null-terminated) to specify the standard used.
Further data then follows within the payload.

### The Payload
In this part, any data that does not belong directly to the header or extended header is sent.
If there is no extended header, the default format of the payload is a null-terminated string.