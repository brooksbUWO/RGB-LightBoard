// ****************************************************************************
// Title		: LED RGB
// File Name	: 'main.cpp'
// Revision		: 1.0
// Target MCU	: Espressif ESP32 (Doit DevKit Version 1)
//
//  Example program to show both how to use FastLed library and some basic
//  programming concepts for the Engineering Club.
//
// Revision History:
// When			    Who	    Description of change
// -----------	-----------	-----------------------
// 22-FEB-2023	brooks		  Start of program
//
// ****************************************************************************

// Include Files
// ****************************************************************************
#include <Arduino.h>

#include <FastLED.h>
#define RGB_NUM 50							// RGB LEDs quantity
#define RGB_PIN 4							// RGB data pin
CRGB leds[RGB_NUM];							// Declare RGB structure

unsigned long timePrev = millis();			// Non-Blocking LED heartbeat
unsigned long time10 = millis();			// Non-Blocking timing

uint8_t ledState = LOW;						// State of LED

uint8_t counter = 0;						// Counter

// Begin Code
// ****************************************************************************
void setup() 
{
	Serial.begin(115200);					// Serial monitor baud rate 115200				
	pinMode(LED_BUILTIN, OUTPUT);			// Set pin to function as an output

	FastLED.addLeds<WS2811, RGB_PIN, RGB>(leds, RGB_NUM);
}


// Main program
// ****************************************************************************
void loop() 
{
	if ( millis()-timePrev >= 1000 )		// Repeats every 1sec
	{
		timePrev = millis();				// Reset time
		ledState = !ledState;				// Toggle LED on/off
		digitalWrite(LED_BUILTIN, ledState);	

		// Turn on all 50 leds
		for (uint8_t i=0; i<RGB_NUM; i++)
		{
			leds[i] = CRGB::Yellow;
		 	FastLED.setBrightness(28);		
	 		FastLED.show();			
		}
	}

	// This turns on multiple colors
	// if ( millis()-time10 >= 100 )			// Repeats every 100ms
	// {
	// 	time10 = millis();					// Reset time

	// 	leds[counter+0] = CRGB::Black;
	// 	leds[counter+1] = CRGB::Blue;
	// 	leds[counter+2] = CRGB::Yellow;
	// 	leds[counter+3] = CRGB::Red;
	// 	leds[counter+4] = CRGB::Green;		
	// 	FastLED.setBrightness(28);		
	// 	FastLED.show();

	// 	counter = counter + 1;
	// 	if ( counter >= 50 )
	// 		counter = 0;
	// }

}