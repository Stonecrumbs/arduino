/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks 
  ----> http://www.adafruit.com/products/872
  ----> http://www.adafruit.com/products/871
  ----> http://www.adafruit.com/products/870

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
  matrix.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address
}

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    
    B01000010,
    B00111100 };

    byte eyeBall[18][8]={{B00111100,B01111110,B11111111,B11111111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11001111,B11001111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11001111,B11001111,B11111111,B11111111,B01111110,B00111100},
                        
                        {B00000000,B00111100,B01111110,B11111111,B11111111,B11111111,B01111110,B00111100},
                        {B00000000,B00000000,B00111100,B11111111,B11111111,B11111111,B01111110,B00111100},
                        {B00000000,B00000000,B00111100,B11111111,B11111111,B11111111,B01111110,B00111100},
                        {B00000000,B00000000,B00000000,B00111100,B11111111,B11111111,B00111100,B00000000},
                        {B00000000,B00000000,B00000000,B00000000,B00111100,B11111111,B00111100,B00000000},
                        {B00000000,B00000000,B00000000,B00000000,B00111100,B00111100,B00000000,B00000000},
                        
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11111001,B11111001,B11111111,B11111111,B01111110,B00111100}};

                        

    byte eyeBall2[18][8]={{B00111100,B01111110,B11111111,B11111111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        
                        {B00000000,B00111100,B01111110,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00000000,B00000000,B00111100,B11111111,B11111111,B11111111,B01111110,B00111100},
                        
                        {B00000000,B00000000,B00111100,B11111111,B11111111,B11111111,B01111110,B00111100},
                        {B00000000,B00000000,B00000000,B00111100,B11111111,B11111111,B00111100,B00000000},
                        {B00000000,B00000000,B00000000,B00000000,B00111100,B11111111,B00111100,B00000000},
                        {B00000000,B00000000,B00000000,B00000000,B00111100,B00111100,B00000000,B00000000},
                        
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B10011111,B10011111,B11111111,B11111111,B01111110,B00111100},
                        
                        {B00111100,B01111110,B11100111,B11100111,B11111111,B11111111,B01111110,B00111100},
                        {B00111100,B01111110,B11111001,B11111001,B11111111,B11111111,B01111110,B00111100}};
  

void loop() {
/*
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();

  matrix2.clear();
  matrix2.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_YELLOW);
  matrix2.writeDisplay();
  delay(500);
*/
  for (int i = 0; i < 15; i++){
    matrix.clear();
    matrix.drawBitmap(0, 0, eyeBall[i], 8, 8, LED_GREEN);
    matrix.writeDisplay();    
    matrix2.clear();
    matrix2.drawBitmap(0, 0, eyeBall2[i], 8, 8, LED_GREEN);
    matrix2.writeDisplay();
    delay(80);
  }
}
