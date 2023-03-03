// ****************************************************************************
// Title		: LED RGB
// File Name	: 'main.cpp'
// Revision		: 1.0
// Target MCU	: Espressif ESP32 (Doit DevKit Version 1)
//
//  Program for the Engineering Club using io abstraction, FastLed library,
//  and LED matrix control.
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

// FastLed library
// https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#one-array-many-strips
#include <FastLED.h>
#define NUM_STRIPS 7
#define NUM_LEDS_PER_STRIP 50
#define NUM_LEDS 950
//#define NUM_LEDS ( (NUM_LEDS_PER_STRIP * NUM_STRIPS) - 126)
CRGB leds[NUM_LEDS];

// LCD hd44780
// https://reference.arduino.cc/reference/en/libraries/hd44780/
#include <Wire.h>
#include <hd44780.h>						// Main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h>	// I2C expander I/O class header
hd44780_I2Cexp lcd;							// LCD object config expander chip
const int LCD_COLS = 20;					// LCD columns
const int LCD_ROWS = 4;						// LCD rows


// Functions
// ****************************************************************************
void initLCD()
{
	int status;
	// initialize LCD with number of columns and rows:
	// hd44780 returns a status from begin() that can be used
	// to determine if initalization failed.
	// the actual status codes are defined in <hd44780.h>
	// See the values RV_XXXX
	//
	// looking at the return status from begin() is optional
	// it is being done here to provide feedback should there be an issue
	//
	// note:
	//	begin() will automatically turn on the backlight
	//
	status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		// hd44780 has a fatalError() routine that blinks an led if possible
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}
}

// Begin Code
// ****************************************************************************
void setup() 
{
	Serial.begin(115200);					// Serial monitor baud rate 115200
	Serial.println();

	pinMode(LED_BUILTIN, OUTPUT);	// Set pin to function as an OUTPUT

	FastLED.addLeds<WS2811, 4, RGB>(leds, 0, NUM_LEDS);

	initLCD();
}


// Main program
// ****************************************************************************
void loop() 
{

	if ( millis()-timePrev >= 1000 )		// Repeats every 1sec
	{
		timePrev = millis();				// Reset time delay
		ledState = !ledState;				// Toggle LED on/off
		digitalWrite(LED_BUILTIN, ledState);

		lcd.setCursor(0,0);
		lcd.print("This is LCD Test");
		lcd.setCursor(0,2);
		lcd.print("timePrev=");
		lcd.print(String(timePrev));
	}

	if ( millis()-timeDelay >= 50 )			// Repeats every 50ms
	{
		timeDelay = millis();				// Reset time delay

		leds[counter] = CRGB::Red;
		FastLED.setBrightness(28);			
		FastLED.show();

		counter = counter + 1;
		if ( counter >= (NUM_LEDS) )
			counter = 0;

		lcd.setCursor(0,1);
		lcd.print("millis=");
		lcd.print(String(millis()));

		lcd.setCursor(0,3);
		lcd.print("Led Counter=");
		lcd.print(String(counter));
	}


}