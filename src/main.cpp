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
//
//
// ****************************************************************************


// Include Files
// ****************************************************************************
#include <Arduino.h>
#include <main.h>

unsigned long timePrev = millis();			// Non-Blocking LED heartbeat
uint8_t ledState = LOW;						// State of LED

unsigned long time1 = millis();				// Non-Blocking timing delay
unsigned long timeDelay1 = 70000;			// Time in milliseconds
bool flagRun1 = false;						// Flag for time period

unsigned long time2 = millis();				// Non-Blocking timing delay
unsigned long timeDelay2 = timeDelay1*2;	// Time in milliseconds
bool flagRun2 = false;						// Flag for time period

unsigned long time3 = millis();				// Non-Blocking timing delay
unsigned long timeDelay3 = timeDelay2*2;	// Time in milliseconds
bool flagRun3 = false;						// Flag for time period

// LCD hd44780
// https://reference.arduino.cc/reference/en/libraries/hd44780/
#include <Wire.h>
#include <hd44780.h>						// Main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h>	// I2C expander I/O class header
hd44780_I2Cexp lcd;							// LCD object config expander chip
const int LCD_COLS = 20;					// LCD columns
const int LCD_ROWS = 4;						// LCD rows

// FastLed library
// https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#one-array-many-strips
#include <FastLED.h>
#define NUM_LEDS 	924
#define DATA_PIN 	4						// RGB led data pin
#define COLOR_ORDER RGB						// RGB led color order
#define CHIPSET 	WS2811					// RGB led type
CRGB leds[NUM_LEDS];

// RGB Matrix, Text & Sprite libraries
// https://github.com/AaronLiddiment/RGBLEDS
// https://github.com/AaronLiddiment/LEDText/wiki/1.Setting-Up
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontCourierNew7x11.h>
#define MATRIX_WIDTH   42
#define MATRIX_HEIGHT  -22
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX
// Instantiate an instance of the cLEDMatrix class
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> matrix;
cLEDText display;						// Declare cLEDText class variable

uint8_t hue = 0;
int16_t counter = 0;

// Various things to display on the RGB matrix
const unsigned char TxtDemo[] = { 
	EFFECT_SCROLL_LEFT "            LEFT SCROLL "
    EFFECT_SCROLL_RIGHT "            LLORCS THGIR"
	EFFECT_SCROLL_DOWN "            SCROLL DOWN             SCROLL DOWN            " EFFECT_FRAME_RATE "\x04" " SCROLL DOWN            " EFFECT_FRAME_RATE "\x00" " "
	EFFECT_SCROLL_UP "             SCROLL UP               SCROLL UP             " EFFECT_FRAME_RATE "\x04" "  SCROLL UP             " EFFECT_FRAME_RATE "\x00" " "
	EFFECT_CHAR_UP EFFECT_SCROLL_LEFT "            UP"
	EFFECT_CHAR_RIGHT "  RIGHT"
	EFFECT_CHAR_DOWN "  DOWN"
	EFFECT_CHAR_LEFT "  LEFT"
	EFFECT_HSV_CV "\x00\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           HSV_CV 00-40"
	EFFECT_HSV_CH "\x00\xff\xff\x40\xff\xff" "    HSV_CH 00-40"
	EFFECT_HSV_AV "\x00\xff\xff\x40\xff\xff" "    HSV_AV 00-40"
	EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "    HSV_AH 00-FF"
	"           " EFFECT_HSV "\x00\xff\xff" "R" EFFECT_HSV "\x20\xff\xff" "A" EFFECT_HSV "\x40\xff\xff" "I" EFFECT_HSV "\x60\xff\xff" "N" EFFECT_HSV "\xe0\xff\xff" "B" EFFECT_HSV "\xc0\xff\xff" "O"
	EFFECT_HSV "\xa0\xff\xff" "W" EFFECT_HSV "\x80\xff\xff" "S " EFFECT_DELAY_FRAMES "\x00\x96" EFFECT_RGB "\xff\xff\xff" 
};


const unsigned char displayMe[] = {
	EFFECT_FRAME_RATE "\x04" EFFECT_SCROLL_UP "UWO"
	EFFECT_FRAME_RATE "\x04" EFFECT_SCROLL_DOWN "UWO"
	EFFECT_FRAME_RATE "\x04" EFFECT_SCROLL_LEFT "UWO"
	EFFECT_DELAY_FRAMES "\x00\x96"
};


// Functions
// ****************************************************************************
void initLCD()
{
	int status;
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

	initLCD();								// Initialize LCD display
	pinMode(LED_BUILTIN, OUTPUT);			// Set pin function as an OUTPUT

	FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
	// Test matrix on start-up by displaying some colors
  	FastLED.setBrightness(255);	
	FastLED.clear(true);
	FastLED.showColor(CRGB::Red);
	delay(500);
	FastLED.showColor(CRGB::Lime);
	delay(500);
	FastLED.showColor(CRGB::Blue);
	delay(500);
	FastLED.showColor(CRGB::White);
	delay(500);
  	FastLED.clear(true);
	FastLED.show();

	display.SetFont(FontCourierNew7x11Data);
	display.Init(&matrix, matrix.Width(), display.FontHeight() + 1, 0, 0);
	display.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
	//display.SetText((unsigned char *)displayMe, sizeof(displayMe) - 1);
	display.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0xff, 0x00);	// Yellow
	
	time1 = millis();
	time2 = millis();
}


// Main program (example display text)
// ****************************************************************************
void loop() 
{

	if ( millis()-timePrev >= 1000 )		// Repeats every 1sec
	{
		timePrev = millis();				// Reset time delay
		ledState = !ledState;				// Toggle LED on/off
		digitalWrite(LED_BUILTIN, ledState);

		lcd.setCursor(0,0);
		lcd.print("UWO Lightboard");
		lcd.setCursor(0,1);
		lcd.print("TimePrev=");
		lcd.print(String(timePrev));
		lcd.print("ms");
	}

	if (display.UpdateText() == -1)
		display.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
		//display.SetText((unsigned char *)displayMe, sizeof(displayMe) - 1);
	else if ( (millis()-time1 >= timeDelay1) )
	{
		drawStuff();
	}
	else
		FastLED.show();

}


// Draw shapes and stuff
// ****************************************************************************
void drawStuff() 
{
  int16_t sx, sy, x, y;
  uint8_t h;

  FastLED.clear();
  
  h = hue;
  if (counter < 1125)
  {
    // ** Fill LED's with diagonal stripes
    for (x=0; x<(matrix.Width()+matrix.Height()); ++x)
    {
      matrix.DrawLine(x - matrix.Height(), matrix.Height() - 1, x, 0, CHSV(h, 255, 255));
      h+=16;
    }
  }
  else
  {
    // ** Fill LED's with horizontal stripes
    for (y=0; y<matrix.Height(); ++y)
    {
      matrix.DrawLine(0, y, matrix.Width() - 1, y, CHSV(h, 255, 255));
      h+=16;
    }
  }
  hue+=4;

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