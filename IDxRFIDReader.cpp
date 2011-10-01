/* IDxRFIDReader.cpp
 * v0.1 11/15/09
 * v0.2 09/18/10 - updated to implement Stream base class
 *
 * Arduino Library for Innovations ID-x series RFID Readers
 *
 * Copyright (C) 2010 Jeff Hoefs.  All rights reserved.
 *
 * Some code snippits taken from here:
 * http://www.arduino.cc/playground/Code/ID12
 *
 * To do:
 * 1. Handle card swapping. It is currently possible (but somewhat difficult)
 *	  to swap tags between reads so that a different tag will be reported as
 *	  being removed than the tag that was reported as being added.
 * 2. Come up with a better way to check for a tag after reset. The current
 *    approach using tagRemoveTimeCheck could be difficult for users because
 *    they will need to fine tune this value if they have a long main loop
 *    in their applicaiton.
 * 3. Lighten this library up a bit if possible.
 *
 * Tested successfully with ID-12 Reader
 *
 */

#include <WProgram.h>
#include "IDxRFIDReader.h"

//*******************************************************************************
//* Constructor
//*******************************************************************************
IDxRFIDReader::IDxRFIDReader(Stream &softSerial, int resetPin) {
	
	serialObject = &softSerial;
	
	isTagPresent = 0;
	isReset = 0;
	reset_pin = resetPin;
	tagRemoveTimeCheck = TAG_REMOVE_TIME_CHECK;
	
	if(resetPin > -1) {
		pinMode(resetPin, OUTPUT);
		digitalWrite(resetPin, HIGH);
	}
}

//*******************************************************************************
//* Public Methods
//*******************************************************************************
void IDxRFIDReader::update(void) {
	// check to see if data is available from the reader
	// if it is process the data and set tagValue
	if(checkForRFIDData()) {
		// if reset is enabled reset the Reader so we can 
		// see if the tag is still present
		if(reset_pin > -1) resetReader();
		else {
			// if a tag read is valid, print the value
			setTag(tagDataArray, tag);
			if(currentReadTagCallback)
				(*currentReadTagCallback)(tag);
		}
	} else {
		// if the reader was reset and TAG_REMOVE_TIME_CHECK milliseconds
		// have passed, then there is no longer a tag present.
		// The value of TAG_REMOVE_TIME_CHECK should account for the time
		// it takes the reader to reset and the time it takes your main
		// loop to run.
		// So if you have a lot going on in your main loop, make this value
		// a lot higher.
		if(reset_pin > -1) checkResetTimer();
	}

}

void IDxRFIDReader::reset(int pin) {
	if(pin > -1) {
		digitalWrite(pin, LOW);
		digitalWrite(pin, HIGH);
	} else {
		// to do: error, need to specify reset pin
	}
}

void IDxRFIDReader::attach(byte eventType, readerCallbackFunction newFunction) {
	switch(eventType) {
		case READ_TAG: currentReadTagCallback = newFunction; break;
		case REMOVE_TAG: currentRemoveTagCallback = newFunction; break;
	}
}

void IDxRFIDReader::detach(byte eventType) {
  switch(eventType) {
  case READ_TAG: currentReadTagCallback = NULL; break;
  case REMOVE_TAG: currentRemoveTagCallback = NULL; break;
  default:
    attach(eventType, (readerCallbackFunction)NULL);
  }
}


// time to wait after resetting the reader to check
// if there is still a tag present
// will need to increase this value if your main loop takes
// a while to run
void IDxRFIDReader::setTimeCheckDuration(unsigned long duration) {
	tagRemoveTimeCheck = duration;
}

//*******************************************************************************
//* Private Methods
//*******************************************************************************
void IDxRFIDReader::setTag(byte *dataArray, byte *tagArray) {
	// copy 1st 5 bytes from dataArray into tagArray
	for(byte i=0; i<5; i++) {
		tagArray[i] = dataArray[i];
	}
}

void IDxRFIDReader::resetReader(void) {
	if(!isReset) {
		setTag(tagDataArray, tag);
		if(currentReadTagCallback)
				(*currentReadTagCallback)(tag);
	}
	
	reset(reset_pin);
	isReset = 1;
	// store current value of millis() so we can check if the
	// tag is still present without needing to use a delay()
	resetCounterLastMillis = millis();  
}

void IDxRFIDReader::checkResetTimer(void) {
	if(isReset && millis() - resetCounterLastMillis >= tagRemoveTimeCheck) {
		setTag(tagDataArray, tag);
		if(currentRemoveTagCallback)
				(*currentRemoveTagCallback)(tag);
		isReset = 0;   
	}
}

byte IDxRFIDReader::checkForRFIDData(void) {
  byte counter = 0;
  byte returnValue = 0;
  if(serialObject->available()) {
         
     while(serialObject->available()) {
         returnValue = processData(serialObject->read());
         
         // escape route
         if(counter++ > 16) break; 
     } 

   }
   return returnValue; 
}

// read each ASCII value returned by the ID-12
// pack pairs of ASCII Hex values into single bytes
// test the checksum and if successful, store the
// rfid tag value
byte IDxRFIDReader::processData(byte inputData) {
    static byte tempValue;
    static byte counter;
    byte decimalValue;
    byte returnValue = 0;
    
    // check for start transmission
    if(inputData == STX) {
        counter = 0; 
    } else {
        // check for end of transmission
        if(inputData == ETX) {
            // if the checksum is correct, print the tag 
            if(tagDataArray[5] == calculateChecksum(tagDataArray)) {
              returnValue = 1;
            } else {
              //to do: pass error back to main application
            }
        } else {
            // ignore line feed and carriage return
            if(inputData != LF && inputData != CR) {
                 // get the decimal values of the hex characters returned
                 // by the RFID reader in ASCII format
                 decimalValue = convertASCIIHexToDecimal(inputData);
                 
                 // pack every 2 ASCII hex values into a single byte value
                 if(counter % 2) {
                     tagDataArray[counter >> 1] = (decimalValue | (tempValue << 4));
                 } else tempValue = decimalValue;
            } 
        }
        counter++;
    }
    return returnValue;
}

// Exclusive OR the 5 data bytes returned by the ID-12
// the resulting value should equal the checksum (byte 6 of the array)
byte IDxRFIDReader::calculateChecksum(byte *dataArray) {
    byte checksum = 0;
    
    // "Exclusive OR" of the 5 hex bytes
    for(byte i=0; i<5; i++) {
       checksum ^= dataArray[i];
    }
    return checksum;
}

// need to convert the ASCII hex values returned from the RFID reader
// to decimal values (actually decimal values 0 - 15)
// to do: perhaps there is a more elegent way to do this?
byte IDxRFIDReader::convertASCIIHexToDecimal(byte ASCIIValue) {
    byte hexValue;
    
    if(ASCIIValue >= '0' && ASCIIValue <= '9')
        hexValue = ASCIIValue - '0';
    else if(ASCIIValue >='A' && ASCIIValue <= 'F')
        hexValue = 10 + (ASCIIValue - 'A');
        
    return hexValue;
}