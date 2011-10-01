/*
 * Example sketch for IDxRFIDReader library
 *
 * Tested with ID-12 Rfid reader, but should work with
 * any of the ID-x readers
 *
 * Supported for Arduino versions 1.0 and higher
 * 
 * Jeff Hoefs
 *
 * ID-12 Pin connections:
 * 1: GND
 * 2: 470 ohm resistor to Arduino pin 4
 * 7: GND
 * 9: RX (to Arduino pin 2)
 * 10: 330 ohm resistor to LED
 * 11: VCC
 * All other pins on ID-12 are not connected
 */

#include <SoftwareSerial.h>  
#include <IDxRFIDReader.h>
#include <Firmata.h>

// define rx and tx pins for software serial
#define RX_PIN           0X02
#define TX_PIN           0x03
#define RESET_PIN        0x04

#define SYSEX_ID12       0x0D

byte tagDataFirmata[6];

SoftwareSerial softSerial(RX_PIN, TX_PIN);

// create instance of IDx Reader 
IDxRFIDReader ID12Reader(softSerial, RESET_PIN);


void setup() {
  
  Firmata.setFirmwareVersion(2, 3);
  Firmata.attach(START_SYSEX, sysexCallback);  
  Firmata.begin(57600);
  
  // baud rate for ID12 is 9600
  softSerial.begin(9600);  
  
  // callbacks
  ID12Reader.attach(READ_TAG, onReadTag);
  ID12Reader.attach(REMOVE_TAG, onRemoveTag);

}


void loop() {
  
    ID12Reader.update();
    
    while(Firmata.available()) {
        Firmata.processInput(); 
    }    
 
}

void onReadTag(byte *tagValue) {
    tagDataFirmata[0] = READ_TAG;          
    tagDataFirmata[1] = tagValue[0];
    tagDataFirmata[2] = tagValue[1];
    tagDataFirmata[3] = tagValue[2];
    tagDataFirmata[4] = tagValue[3];
    tagDataFirmata[5] = tagValue[4];
    Firmata.sendSysex(SYSEX_ID12, 6, tagDataFirmata);
}

void onRemoveTag(byte *tagValue) {
    tagDataFirmata[0] = REMOVE_TAG;
    tagDataFirmata[1] = tagValue[0];
    tagDataFirmata[2] = tagValue[1];
    tagDataFirmata[3] = tagValue[2];
    tagDataFirmata[4] = tagValue[3];
    tagDataFirmata[5] = tagValue[4];    
    Firmata.sendSysex(SYSEX_ID12, 6, tagDataFirmata);  
}

void sysexCallback(byte command, byte argc, byte *argv)
{
  byte mode;

  if (command == SYSEX_ID12) {
    mode = argv[0];
  } 
}






