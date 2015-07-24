EMBEDDED MESSAGE TRANSPORT PROTOCOL
===================================

The Embedded Message Transport Protocol (EMTP) is a transport protocol
that allows embedding messages, or packets, into a stream of data.

A special ascii character is inserted at the beginning of a message,
ascii code Data Link Escape (0x10). This ascii code is reserved for this purpose
and may _not_ be sent otherwise.

MESSAGE FORMAT
==============

The message format has four fields; begin, type, size and data.

    +-------+------+------+------+
    | begin | type | size | data |
    +-------+------+------+------+

    begin (1 octet):
        must be set to 0x10

    type  (1 octet):
        user specific

    size  (2 octets):
        size of message inxluding the header

    data  (<size> - 4 octets):
        user data

EXAMPLE
=======

In this example we want to send the stream data `"Hello the weather is
nice today!"` and the message `"\x10\x00\x00\x04"`. The message is
sent shortly after the stream data and is inserted in the middle of
it. Where the message is actually inserted is implementation specific.

    "Hello the wea\x10\x00\x00\x04ther is nice today!"
     ----------->||<------------>||<-----------------
       stream          message           stream