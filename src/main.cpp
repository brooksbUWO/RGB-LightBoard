// ****************************************************************************
// Title		: LED RGB
// File Name	: 'main.cpp'
// Revision		: 1.0
// Target MCU	: Espressif ESP32 (Doit DevKit Version 1)
//
//  Example program for the Engineering Club using io abstraction, 
//  FastLed library, and LED matrix control.
//
// Revision History:
// When			    Who	    Description of change
// -----------	-----------	-----------------------
// 22-FEB-2023	brooks		  Start of program
//
//
// ****************************************************************************


// Include Files
// ****************************************************************************
#include <Arduino.h>
unsigned long timePrev = millis();			// Non-Blocking LED heartbeat
unsigned long timeDelay = millis();			// Non-Blocking timing delay
uint8_t ledState = LOW;						// State of LED
uint16_t counter = 0;						// Counter

// https://www.thecoderscorner.com/products/arduino-downloads/io-abstraction/
#include <IoAbstraction.h>
#include <IoAbstractionWire.h>
#include <TaskManagerIO.h>
#include <Wire.h>
MultiIoAbstraction multiIo(100);			// Allocate 100 pins to arduino pins
PCF8574IoAbstraction expander1(0x20, IO_PIN_NOT_DEFINED);
PCF8574IoAbstraction expander2(0x21, IO_PIN_NOT_DEFINED);

// https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#one-array-many-strips
#include <FastLED.h>
#define NUM_STRIPS 22
#define NUM_LEDS_PER_STRIP 42
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];


// Begin Code
// ****************************************************************************
void setup() 
{
	Serial.begin(115200);					// Serial monitor baud rate 115200
	multiIo.pinMode(LED_BUILTIN, OUTPUT);	// Set pin to function as an OUTPUT

	Wire.begin();
	multiIo.addIoDevice(expander1, 16);		// Add 8575 with 16 pins (100-115)
	multiIo.addIoDevice(expander2, 16);		// Add 8575 with 16 pins (116-131)

	for(int i=100; i<132; i++)
	{
		multiIo.pinMode(i, OUTPUT);			// Set pin to function as an OUTPUT
	}

	// Pin abstraction needs line 358 in clockless_rmt_esp32.h commented out
	// expander1 pin P0 abstracted to pin 100, starting at index 0 in led array
	FastLED.addLeds<WS2811, 100, RGB>(leds, 0, NUM_LEDS_PER_STRIP);
	// expander1 pin P1 abstracted to pin 101, starting at index 42 in led array
	FastLED.addLeds<WS2811, 101, RGB>(leds, NUM_LEDS_PER_STRIP*1, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 102, RGB>(leds, NUM_LEDS_PER_STRIP*2, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 103, RGB>(leds, NUM_LEDS_PER_STRIP*3, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 104, RGB>(leds, NUM_LEDS_PER_STRIP*4, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 105, RGB>(leds, NUM_LEDS_PER_STRIP*5, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 106, RGB>(leds, NUM_LEDS_PER_STRIP*6, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 107, RGB>(leds, NUM_LEDS_PER_STRIP*7, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 108, RGB>(leds, NUM_LEDS_PER_STRIP*8, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 109, RGB>(leds, NUM_LEDS_PER_STRIP*9, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 110, RGB>(leds, NUM_LEDS_PER_STRIP*10, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 111, RGB>(leds, NUM_LEDS_PER_STRIP*11, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 112, RGB>(leds, NUM_LEDS_PER_STRIP*12, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 113, RGB>(leds, NUM_LEDS_PER_STRIP*13, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 114, RGB>(leds, NUM_LEDS_PER_STRIP*14, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, 115, RGB>(leds, NUM_LEDS_PER_STRIP*15, NUM_LEDS_PER_STRIP);	

	// expander2 pin P0 abstracted to pin 116, starting at index 714 in led array
	FastLED.addLeds<WS2811, 116, RGB>(leds, NUM_LEDS_PER_STRIP*16, NUM_LEDS_PER_STRIP);	
	// expander2 pin P1 abstracted to pin 117, starting at index 756 in led array
	FastLED.addLeds<WS2811, 117, RGB>(leds, NUM_LEDS_PER_STRIP*17, NUM_LEDS_PER_STRIP);	
	FastLED.addLeds<WS2811, 118, RGB>(leds, NUM_LEDS_PER_STRIP*18, NUM_LEDS_PER_STRIP);	
	FastLED.addLeds<WS2811, 119, RGB>(leds, NUM_LEDS_PER_STRIP*19, NUM_LEDS_PER_STRIP);	
	FastLED.addLeds<WS2811, 120, RGB>(leds, NUM_LEDS_PER_STRIP*20, NUM_LEDS_PER_STRIP);	
	FastLED.addLeds<WS2811, 121, RGB>(leds, NUM_LEDS_PER_STRIP*21, NUM_LEDS_PER_STRIP);	
	FastLED.addLeds<WS2811, 122, RGB>(leds, NUM_LEDS_PER_STRIP*22, NUM_LEDS_PER_STRIP);	

}


// Main program
// ****************************************************************************
void loop() 
{
	taskManager.runLoop();					// Required for IoAbstraction

	if ( millis()-timePrev >= 1000 )		// Repeats every 1sec
	{
		timePrev = millis();				// Reset time delay
		ledState = !ledState;				// Toggle LED on/off
		multiIo.digitalWrite(LED_BUILTIN, ledState);
	}

	if ( millis()-timeDelay >= 50 )			// Repeats every 50ms
	{
		timeDelay = millis();				// Reset time delay

		leds[counter] = CRGB::Black;
		leds[counter+1] = CRGB::Red;
		FastLED.setBrightness(28);			
		FastLED.show();

		counter = counter + 1;
		if ( counter >= (NUM_STRIPS * NUM_LEDS_PER_STRIP) )
			counter = 0;		
	}

}