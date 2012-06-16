IDxRFIDReader
===

Arduino library for the Innovations ID-X series RFID readers. Tested on the ID-12, but should work with other readers. This library is only supported for Arduino 1.0 and higher.

An event is dispatched when a tag is read and also when the tag is removed from the reader. You can attach a listener to each event.

Included in the library package in an example for using the reader with Arduino hardware serial as well as with software serial.

Also included are examples for sending data from the reader to a computer using the Firmata protocol. IDxReader_Firmata_Example uses Firmata as a protocol to send data, and RFID_Firmata is a version of StandardFirmata that includes support for the IDxRFID Reader. For an Actionscript 3.0 example (using funnel - funnel.cc), see my [Funnel Examples repository](https://github.com/soundanalogous/Funnel-AS3-Examples/tree/master/Custom_Examples).
For a javascript example, see [Breakout](https://github.com/soundanalogous/Breakout).

To do:

- Update RFID_Firmata so user can assign pins from the client library (currently pins 2, 3, and 4 are fixed regardless of board type).
