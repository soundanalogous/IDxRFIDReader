/* IDxRFIDReader.h
 * v0.3 09/29/11 - updated for Arduino 1.0
 * v0.2 09/18/10 - updated to implement Stream base class
 * v0.1 11/15/09
 * 
 * Arduino Library for Innovations ID-x series RFID Readers
 *
 * Copyright (C) 2010-2011 Jeff Hoefs.  All rights reserved.
 *
 */

#ifndef IDxRFIDReader_h
#define IDxRFIDReader_h

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Stream.h"

// ascii codes for ID-x
#define STX				0x02
#define ETX				0x03
#define LF				0x0A
#define CR				0x0D

#define READ_TAG		0x01
#define REMOVE_TAG		0x02

#define TAG_REMOVE_TIME_CHECK  500

extern "C" {
// callback function types
	typedef void (*readerCallbackFunction)(byte *data);
}

class IDxRFIDReader {

public:
	IDxRFIDReader(Stream& softSerial, int resetPin = -1);
	byte tag[5];
	
	// public methods
	void update(void);
	void reset(int pin);
	
	void attach(byte eventType, readerCallbackFunction newFunction);
	void detach(byte eventType);
	void setTimeCheckDuration(unsigned long duration);
	
private:
	byte tagDataArray[6];
	byte isTagPresent;
	byte isReset;
	unsigned long resetCounterLastMillis;
	int reset_pin;
	unsigned long tagRemoveTimeCheck;
	
	Stream* serialObject;
	
	// private methods
	void resetReader(void);
	void checkResetTimer(void);
	void setTag(byte *tagDataArray, byte *tag);
	byte checkForRFIDData(void);
	byte processData(byte inputData);
	byte calculateChecksum(byte *dataArray);
	byte convertASCIIHexToDecimal(byte ASCIIValue);
	
	// callback functions
	readerCallbackFunction currentReadTagCallback;
	readerCallbackFunction currentRemoveTagCallback;

};


#endif /* IDxRFIDReader_h */
