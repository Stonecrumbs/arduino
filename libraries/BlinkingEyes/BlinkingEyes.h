/*
  BlinkingEyes.h - Library for drawing a blinking eyes in two 8x8 color matrix.
  Created by Enrique P. Medina, April 11, 2019.
  Released into the public domain.
*/
#ifndef BlinkingEyes_h
#define BlinkingEyes_h

#include "Arduino.h"
#include "Adafruit_GFX.h"

class Morse
{
  public:
    BlinkingEyes(int pin);
    void dot();
    void dash();
  private:

    Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
    Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();

    int _eyesVelocity = 160, /* This variable should be initialised to get the desired velocity for blinking and pupile movement*/
        _pupilePosition = 3,
        _movePupileTo = 3,
        _finalPupilePosition,
        _blinkIteration = 0,
        _eyesFrameControl,
        _eyesColor;

    static const uint8_t   _init_bmp []=
						    {B00011000,
						     B01111110,
						     B11111111,
						     B11111111,
						     B11111111,
						     B11111111,
						     B01111110,
						     B00011000},
						  _blink_it1_bmp []=
						    {B00000000,
						     B00011000,
						     B01111110,
						     B11111111,
						     B11111111,
						     B11111111,
						     B01111110,
						     B00011000},
						  _blink_it2_bmp []=
						    {B00000000,
						     B00000000,
						     B00111100,
						     B11111111,
						     B11111111,
						     B01111110,
						     B00000000,
						     B00000000},
						  _blink_it3_bmp []=
						    {B00000000,
						     B00000000,
						     B00000000,
						     B11111111,
						     B11111111,
						     B00000000,
						     B00000000,
						     B00000000};
	boolean _isPupileMoving = false,
	        _isBlinking = false;


#endif