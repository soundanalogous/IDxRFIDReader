/*
 * Example sketch for IDxRFIDReader library
 *
 * Tested with ID-12 Rfid reader, but should work with
 * any of the ID-x readers
 *
 * Supported for Arduino versions 1.0 and higher
 * 
 * Jeff Hoefs
 * 9/29/11 dropping support versions of Arduino prior to 1.0
 * 09/18/10 updated to use Stream base class
 *
 *
 * ID-12 Pin connections:
 * 1: GND
 * 2: 470 ohm resistor to Arduino pin 4
 * 7: GND
 * 9: RX (to Arduino pin 0)
 * 10: 330 ohm resistor to LED
 * 11: VCC
 * All other pins on ID-12 are not connected
 */

#include <SoftwareSerial.h>  
#include <IDxRFIDReader.h>

// define rx and tx pins for software serial
#define RX_PIN           0X02
#define TX_PIN           0x03
// reset pin connected to reset on IDx module
#define RESET_PIN        0x04


SoftwareSerial softSerial(RX_PIN, TX_PIN);

// create instance of IDx Reader (ID-12 in this case)
IDxRFIDReader ID12Reader(Serial, RESET_PIN);


void setup() {
  
  // baud rate for ID12 is 9600
  Serial.begin(9600);
  
  softSerial.begin(57600);
  
  // callbacks
  ID12Reader.attach(READ_TAG, onReadTag);
  ID12Reader.attach(REMOVE_TAG, onRemoveTag);

}


void loop() {
  
    ID12Reader.update();
 
}

void printTag(byte *tagToPrint) {
  for(int i=0; i<5; i++) {
     softSerial.print(tagToPrint[i], HEX); 
  }
}

void onReadTag(byte *tagValue) {
    softSerial.print("got tag: ");
    printTag(tagValue);
    softSerial.println("");
}

void onRemoveTag(byte *tagValue) {
    softSerial.print("removed tag: ");
    printTag(tagValue);
    softSerial.println("");
}



