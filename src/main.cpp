// ****************************************************************************
// Title		: LED RGB
// File Name	: 'main.cpp'
// Revision		: 1.0
// Target MCU	: Espressif ESP32 (Doit DevKit Version 1)
//
//  Program for the Engineering Club using FastLed library
//  and LED matrix control.
//
// Revision History:
// When			    Who	    Description of change
// -----------	-----------	-----------------------
// 22-FEB-2023	brooks		  Start of program
// 13-FEB-2024	brooks		  Added Finite State Machine
//
// ****************************************************************************

// Include Files
// ****************************************************************************
#include <Arduino.h>
#include <main.h>
// #define DEBUG

unsigned long timePrevious = millis(); // Non-Blocking LED heartbeat
uint8_t ledState = LOW;				   // State of LED

// LCD hd44780
// https://reference.arduino.cc/reference/en/libraries/hd44780/
#include <Wire.h>
#include <hd44780.h>					   // Main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // I2C expander I/O class header
hd44780_I2Cexp lcd;						   // LCD object config expander chip
const int LCD_COLS = 20;				   // LCD columns
const int LCD_ROWS = 4;					   // LCD rows

// FastLed library
// https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#one-array-many-strips
#include <FastLED.h>
#define NUM_LEDS 924	// Total number of LED's
#define DATA_PIN 4		// RGB led data pin
#define COLOR_ORDER RGB // RGB led color order
#define CHIPSET WS2811	// RGB led type
CRGB leds[NUM_LEDS];	// Define LED array

// RGB Matrix, Text & Sprite libraries
// https://github.com/AaronLiddiment/RGBLEDS
// https://github.com/AaronLiddiment/LEDText/wiki/1.Setting-Up
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontCourierNew7x11.h>
#define MATRIX_WIDTH 42										 // Width of the LED matrix
#define MATRIX_HEIGHT -22									 // Height of the LED matrix
#define MATRIX_TYPE HORIZONTAL_ZIGZAG_MATRIX				 // Matrix type
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> matrix; // Instantiate matrix variable
cLEDText display;											 // Instantiate display variable
uint8_t hue = 0;											 // Hue value
int16_t counter = 0;										 // Counter
uint8_t angle = 0;											 // Angle

// Finite State Machine (FSM)
// https://github.com/LennartHennigs/SimpleFSM
#include <SimpleFSM.h> // Include the library
SimpleFSM fsm;		   // Create an instance of the FSM

// Various things to display on the RGB matrix
// https://github.com/AaronLiddiment/LEDText/blob/master/examples/TextExample1/TextExample1.ino
const unsigned char TxtDemo[] = {
	EFFECT_SCROLL_LEFT "            LEFT SCROLL " EFFECT_SCROLL_RIGHT "            LLORCS THGIR" EFFECT_SCROLL_DOWN "            SCROLL DOWN             SCROLL DOWN            " EFFECT_FRAME_RATE "\x04"
					   " SCROLL DOWN            " EFFECT_FRAME_RATE "\x00"
					   " " EFFECT_SCROLL_UP "             SCROLL UP               SCROLL UP             " EFFECT_FRAME_RATE "\x04"
					   "  SCROLL UP             " EFFECT_FRAME_RATE "\x00"
					   " " EFFECT_CHAR_UP EFFECT_SCROLL_LEFT "            UP" EFFECT_CHAR_RIGHT "  RIGHT" EFFECT_CHAR_DOWN "  DOWN" EFFECT_CHAR_LEFT "  LEFT" EFFECT_HSV_CV "\x00\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           HSV_CV 00-40" EFFECT_HSV_CH "\x00\xff\xff\x40\xff\xff"
					   "    HSV_CH 00-40" EFFECT_HSV_AV "\x00\xff\xff\x40\xff\xff"
					   "    HSV_AV 00-40" EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff"
					   "    HSV_AH 00-FF"
					   "           " EFFECT_HSV "\x00\xff\xff"
					   "R" EFFECT_HSV "\x20\xff\xff"
					   "A" EFFECT_HSV "\x40\xff\xff"
					   "I" EFFECT_HSV "\x60\xff\xff"
					   "N" EFFECT_HSV "\xe0\xff\xff"
					   "B" EFFECT_HSV "\xc0\xff\xff"
					   "O" EFFECT_HSV "\xa0\xff\xff"
					   "W" EFFECT_HSV "\x80\xff\xff"
					   "S " EFFECT_DELAY_FRAMES "\x00\x96" EFFECT_RGB "\xff\xff\xff"};

// Display text message with various effects
const unsigned char displayMe[] = {
	EFFECT_FRAME_RATE "\x04" EFFECT_SCROLL_UP "UWO" EFFECT_FRAME_RATE "\x04" EFFECT_SCROLL_DOWN "UWO" EFFECT_FRAME_RATE "\x04" EFFECT_SCROLL_LEFT "UWO" EFFECT_DELAY_FRAMES "\x00\x96"};

// Functions
// ****************************************************************************
void initLCD()
{
	int status;
	status = lcd.begin(LCD_COLS, LCD_ROWS);
}

// https://github.com/AaronLiddiment/LEDText/wiki
void state1Setup()
{
	display.Init(&matrix, matrix.Width(), display.FontHeight() + 1, 0, 0); // https://bit.ly/3IiUXqd
	display.SetFont(FontCourierNew7x11Data);							   // https://bit.ly/4bvBvUt
	display.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);		   // https://bit.ly/49AS5AC
	display.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0xff, 0x00);  // https://bit.ly/49clMIp
}

void state1()
{
	if (display.UpdateText() == -1)										// https://bit.ly/49vMn2M
		display.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1); // https://bit.ly/49AS5AC
	else
		FastLED.show();
}

void state2Setup()
{
	display.Init(&matrix, matrix.Width(), display.FontHeight() + 1, 0, 0);
	display.SetFont(FontCourierNew7x11Data);
	display.SetText((unsigned char *)displayMe, sizeof(displayMe) - 1);
	display.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0xff, 0x00); // Yellow
}

void state2()
{
	if (display.UpdateText() == -1)
		display.SetText((unsigned char *)displayMe, sizeof(displayMe) - 1);
	else
		FastLED.show();
}

// Draw shapes and stuff on the RGB matrix
// https://github.com/AaronLiddiment/LEDMatrix/blob/master/examples/MatrixExample1/MatrixExample1.ino
void state3()
{
	int16_t sx, sy, x, y;
	uint8_t h;

	FastLED.clear();

	h = hue;
	if (counter < 1125)
	{
		// ** Fill LED's with diagonal stripes
		for (x = 0; x < (matrix.Width() + matrix.Height()); ++x)
		{
			matrix.DrawLine(x - matrix.Height(), matrix.Height() - 1, x, 0, CHSV(h, 255, 255));
			h += 16;
		}
	}
	else
	{
		// ** Fill LED's with horizontal stripes
		for (y = 0; y < matrix.Height(); ++y)
		{
			matrix.DrawLine(0, y, matrix.Width() - 1, y, CHSV(h, 255, 255));
			h += 16;
		}
	}
	hue += 4;

	if (counter < 125)
		;
	else if (counter < 375)
		matrix.HorizontalMirror();
	else if (counter < 625)
		matrix.VerticalMirror();
	else if (counter < 875)
		matrix.QuadrantMirror();
	else if (counter < 1125)
		matrix.QuadrantRotateMirror();
	else if (counter < 1250)
		;
	else if (counter < 1500)
		matrix.TriangleTopMirror();
	else if (counter < 1750)
		matrix.TriangleBottomMirror();
	else if (counter < 2000)
		matrix.QuadrantTopTriangleMirror();
	else if (counter < 2250)
		matrix.QuadrantBottomTriangleMirror();

	counter++;
	if (counter >= 2250)
		counter = 0;
	FastLED.show();
}

// Draw flags on the RGB matrix
// https://github.com/AaronLiddiment/LEDMatrix/blob/master/examples/MatrixExample2/MatrixExample2.ino
void state4Setup()
{
	// Scottish Flag
	matrix.DrawFilledRectangle(0, 0, matrix.Width() - 1, matrix.Height() - 1, CRGB(0, 0, 255));
	matrix.DrawRectangle(0, 0, matrix.Width() - 1, matrix.Height() - 1, CRGB(255, 255, 255));
	matrix.DrawLine(0, 0, matrix.Width() - 1, matrix.Height() - 1, CRGB(255, 255, 255));
	matrix.DrawLine(0, 1, matrix.Width() - 1, matrix.Height() - 2, CRGB(255, 255, 255));
	matrix.DrawLine(0, matrix.Height() - 1, matrix.Width() - 1, 0, CRGB(255, 255, 255));
	matrix.DrawLine(0, matrix.Height() - 2, matrix.Width() - 1, 1, CRGB(255, 255, 255));
	FastLED.show();
	delay(5000);

	// Japanese Flag
	matrix.DrawFilledRectangle(0, 0, matrix.Width() - 1, matrix.Height() - 1, CRGB(255, 255, 255));
	uint16_t r = min((matrix.Width() - 1) / 2, (matrix.Height() - 1) / 2) - 1;
	matrix.DrawFilledCircle((matrix.Width() - 1) / 2, (matrix.Height() - 1) / 2, r, CRGB(255, 0, 0));
	FastLED.show();
	delay(5000);

	// Norwegian Flag
	int16_t x = (matrix.Width() / 4);
	int16_t y = (matrix.Height() / 2) - 2;
	matrix.DrawFilledRectangle(0, 0, x, y, CRGB(255, 255, 255));
	matrix.DrawFilledRectangle(0, 0, x - 1, y - 1, CRGB(255, 0, 0));
	matrix.DrawFilledRectangle(x + 3, 0, matrix.Width() - 1, y, CRGB(255, 255, 255));
	matrix.DrawFilledRectangle(x + 4, 0, matrix.Width() - 1, y - 1, CRGB(255, 0, 0));
	matrix.DrawFilledRectangle(0, y + 3, x, matrix.Height() - 1, CRGB(255, 255, 255));
	matrix.DrawFilledRectangle(0, y + 4, x - 1, matrix.Height() - 1, CRGB(255, 0, 0));
	matrix.DrawFilledRectangle(x + 3, y + 3, matrix.Width() - 1, matrix.Height() - 1, CRGB(255, 255, 255));
	matrix.DrawFilledRectangle(x + 4, y + 4, matrix.Width() - 1, matrix.Height() - 1, CRGB(255, 0, 0));
	matrix.DrawLine(0, y + 1, matrix.Width() - 1, y + 1, CRGB(0, 0, 255));
	matrix.DrawLine(0, y + 2, matrix.Width() - 1, y + 2, CRGB(0, 0, 255));
	matrix.DrawLine(x + 1, 0, x + 1, matrix.Height() - 1, CRGB(0, 0, 255));
	matrix.DrawLine(x + 2, 0, x + 2, matrix.Height() - 1, CRGB(0, 0, 255));
	FastLED.show();
	delay(5000);
	matrix.ShiftLeft();
}

void state4()
{
	uint8_t h = sin8(angle);
	matrix.ShiftLeft();
	for (int16_t y = matrix.Height() - 1; y >= 0; --y)
	{
		matrix(matrix.Width() - 1, y) = CHSV(h, 255, 255);
		h += 32;
	}
	angle += 4;
	FastLED.show();
}

State s[] = {
	State("textDemo", state1Setup, state1),
	State("displayMe", state2Setup, state2),
	State("drawStuff", state3),
	State("drawFlags", state4Setup, state4)};

TimedTransition timedTransitions[] = {
	TimedTransition(&s[0], &s[1], 1000 * 60 * 1), // 1 minute
	TimedTransition(&s[1], &s[2], 1000 * 60 * 1), // 1 minute
	TimedTransition(&s[2], &s[3], 1000 * 60 * 2), // 2 minutes
	TimedTransition(&s[3], &s[0], 1000 * 60 * 1), // 1 minute
};

int num_timed = sizeof(timedTransitions) / sizeof(TimedTransition);

// Begin Code
// ****************************************************************************
void setup()
{
	Serial.begin(115200);		  // Serial monitor baud rate 115200
	Serial.println();			  // Print a blank line to the serial monitor
	initLCD();					  // Initialize LCD display
	pinMode(LED_BUILTIN, OUTPUT); // Set pin function as an OUTPUT

	FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
	FastLED.setBrightness(255);
	FastLED.clear(true);

#ifdef DEBUG
	// Test matrix on start-up by displaying some colors
	FastLED.showColor(CRGB::Red);
	delay(1000);
	FastLED.showColor(CRGB::Lime);
	delay(1000);
	FastLED.showColor(CRGB::Blue);
	delay(1000);
	FastLED.showColor(CRGB::White);
	delay(1000);
	FastLED.clear(true);
#endif
	FastLED.show();

	fsm.add(timedTransitions, num_timed); // Add timed transitions to the FSM
	fsm.setInitialState(&s[0]);			  // Set the initial state of the FSM
}

// Main program (example display text)
// ****************************************************************************
void loop()
{
	fsm.run(); // run the FSM

	if (millis() - timePrevious >= 1000) // Repeats every 1sec
	{
		ledState = !ledState; // Toggle LED on/off
		digitalWrite(LED_BUILTIN, ledState);

		lcd.setCursor(0, 0);		 // Set cursor to first column, first row
		lcd.print("UWO Lightboard"); // Print message to LCD
		lcd.setCursor(0, 1);		 // Set cursor to first column, second row
		lcd.print("Run time: ");	 // Print message to LCD

		// Calculate hours, minutes, and seconds
		int hours = timePrevious / (1000 * 60 * 60);	 // Divide by 3600 for hours
		int minutes = (timePrevious / (1000 * 60)) % 60; // Divide by 60 for minutes
		int seconds = (timePrevious / 1000) % 60;		 // Divide by 60 for seconds

		// Format run time string (with leading zeros)
		char runTime[9];
		snprintf(runTime, sizeof(runTime), "%02d:%02d:%02d", hours, minutes, seconds);
		lcd.print(String(runTime)); // Print message to LCD
		timePrevious = millis();	// Reset time delay
	}
}