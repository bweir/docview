"
This object adds extra features specific to Parallax serial terminals.

This object is heavily based on FullDuplexSerialPlus (by Andy Lindsay), which is itself
heavily based on FullDuplexSerial (by Chip Gracey).

# Usage

-   Call Start, or StartRxTx, first.
-   Be sure to set the Parallax Serial Terminal software to the baudrate specified in Start, and the proper COM port.
-   At 80 MHz, this object properly receives/transmits at up to 250 Kbaud, or performs transmit-only at up to 1 Mbaud.

Control Character Constants

# Functions

## Start(baudrate) 

Start communication with the Parallax Serial Terminal using the Propeller's programming connection.
Waits 1 second for connection, then clears screen.

Parameters

baudrate -  bits per second.  Make sure it matches the Parallax Serial Terminal's
            Baud Rate field.

Returns True (non-zero) if cog started, or False (0) if no cog is available.

## StartRxTx(rxpin, txpin, mode, baudrate)

Start serial communication with designated pins, mode, and baud.

Parameters

rxpin - input pin; receives signals from external device's TX pin.
txpin - output pin; sends signals to  external device's RX pin.
mode  - signaling mode (4-bit pattern).
           bit 0 - inverts rx.
           bit 1 - inverts tx.
           bit 2 - open drain/source tx.
           bit 3 - ignore tx echo on rx.
baudrate - bits per second.

Returns    
True (non-zero) if cog started, or False (0) if no cog is available.

## Stop

Stop serial communication; frees a cog.

## Count

Get count of characters in receive buffer.

## Flush

Flush receive buffer.

## Char(ch)

Send single-byte character.  Waits for room in transmit buffer if necessary.

## Chars(ch, size)

Send string of size `size` filled with `bytechr`.

## CharIn

Receive single-byte character.  Waits until character received.

## Str(stringptr)

Send zero-terminated string.
Parameter

stringptr - pointer to zero terminated string to send.

## StrIn(stringptr)

Receive a string (carriage return terminated) and stores it (zero terminated) starting at stringptr.
Waits until full string received.

Parameter

stringptr - pointer to memory in which to store received string characters.
            Memory reserved must be large enough for all string characters plus a zero terminator.

## StrInMax(stringptr, maxcount)

Receive a string of characters (either carriage return terminated or maxcount in
length) and stores it (zero terminated) starting at stringptr.  Waits until either
full string received or maxcount characters received.

Parameters

stringptr - pointer to memory in which to store received string characters.
            Memory reserved must be large enough for all string characters plus a zero terminator (maxcount + 1).
maxcount  - maximum length of string to receive, or -1 for unlimited.

## Dec(value)

Send value as decimal characters.
Parameter

value - byte, word, or long value to send as decimal characters.

## DecIn

Receive carriage return terminated string of characters representing a decimal value.

Returns
the corresponding decimal value.

## Bin(value, digits)

Send value as binary characters up to digits in length.

Parameters

value  - byte, word, or long value to send as binary characters.
digits - number of binary digits to send.  Will be zero padded if necessary.

## BinIn

Receive carriage return terminated string of characters representing a binary value.

Returns
the corresponding binary value.

## Hex(value, digits)

Send value as hexadecimal characters up to digits in length.
Parameters

value  - byte, word, or long value to send as hexadecimal characters.
digits - number of hexadecimal digits to send.  Will be zero padded if necessary.

## HexIn

Receive carriage return terminated string of characters representing a hexadecimal value.

Returns
the corresponding hexadecimal value.

## Clear

Clear screen and place cursor at top-left.

## NewLine

Clear screen and place cursor at top-left.

## Position(x, y)

Position cursor at column x, row y (from top-left).

## PositionX(x)

Position cursor at column x of current row.

## PositionY(y)

Position cursor at row y of current column.

## MoveLeft(x)

Move cursor left x characters.

## MoveRight(x)

Move cursor right x characters.

## MoveUp(y)

Move cursor up y lines.

## MoveDown(y)

Move cursor down y lines.

## ReadLine(line, maxline) 

"
