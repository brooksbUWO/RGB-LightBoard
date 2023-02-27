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

#define NUM_STRIPS 22
#define NUM_LEDS_PER_STRIP 42
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#include <PCF8575.h>
TwoWire I2Cone = TwoWire(0);				// Instantiate Wire for use at 400kHz
TwoWire I2Ctwo = TwoWire(0);				// Instantiate Wire for use at 400kHz

// PCF8575 address map 0x20-0x27 use A0, A1, A2 to change address
PCF8575 expander1(&I2Cone, 0x20);				// Set I2C address for I2C one
PCF8575 expander2(&I2Ctwo, 0x21);				// Set I2C address for I2C two

// #define RGB_NUM 50							// RGB LEDs quantity
// #define RGB_PIN 4							// RGB data pin
// CRGB leds[RGB_NUM];							// Declare RGB structure

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

	I2Cone.begin(0x20, 21, 22, 400000);		// Wire begin (address, pins, speed)
	I2Ctwo.begin(0x21, 21, 22, 400000);		// Wire begin (address, pins, speed)

	for(int i=0; i<16; i++)					// Set pinMode to OUTPUT
	{
		expander1.pinMode(i, OUTPUT);
		expander2.pinMode(i, OUTPUT);
	}
	expander1.begin();
	expander2.begin();

	//expander1.digitalWrite();

	// expander1 pin P0, starting at index 0 in led array
	FastLED.addLeds<WS2811, P0, RGB>(leds, 0, NUM_LEDS_PER_STRIP);
	// expander1 pin P1, starting at index 42 in led array	
	FastLED.addLeds<WS2811, P1, RGB>(leds, NUM_LEDS_PER_STRIP*2, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P2, RGB>(leds, NUM_LEDS_PER_STRIP*3, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P3, RGB>(leds, NUM_LEDS_PER_STRIP*4, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P4, RGB>(leds, NUM_LEDS_PER_STRIP*5, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P5, RGB>(leds, NUM_LEDS_PER_STRIP*6, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P6, RGB>(leds, NUM_LEDS_PER_STRIP*7, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P7, RGB>(leds, NUM_LEDS_PER_STRIP*8, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P8, RGB>(leds, NUM_LEDS_PER_STRIP*9, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P9, RGB>(leds, NUM_LEDS_PER_STRIP*10, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P10, RGB>(leds, NUM_LEDS_PER_STRIP*11, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P11, RGB>(leds, NUM_LEDS_PER_STRIP*12, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P12, RGB>(leds, NUM_LEDS_PER_STRIP*13, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P13, RGB>(leds, NUM_LEDS_PER_STRIP*14, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P14, RGB>(leds, NUM_LEDS_PER_STRIP*15, NUM_LEDS_PER_STRIP);
	FastLED.addLeds<WS2811, P15, RGB>(leds, NUM_LEDS_PER_STRIP*16, NUM_LEDS_PER_STRIP);	

	// need to setup direct pin access somehow for each expander
	// lambda function for correct pin on each expander
//	FastLED.addLeds<WS2811, P15, RGB>(leds, NUM_LEDS_PER_STRIP*16, NUM_LEDS_PER_STRIP);	

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

		// // Turn on all 50 leds
		// for (uint8_t i=0; i<RGB_NUM; i++)
		// {
		// 	leds[i] = CRGB::Yellow;
		//  	FastLED.setBrightness(28);		
	 	// 	FastLED.show();			
		// }
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