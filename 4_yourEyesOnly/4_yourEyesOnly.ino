
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();

static const uint8_t PROGMEM
  init_bmp []=
    {B00011000,
     B01111110,
     B11111111,
     B11111111,
     B11111111,
     B11111111,
     B01111110,
     B00011000},
  blink_it1_bmp []=
    {B00000000,
     B00011000,
     B01111110,
     B11111111,
     B11111111,
     B11111111,
     B01111110,
     B00011000},
  blink_it2_bmp []=
    {B00000000,
     B00000000,
     B00111100,
     B11111111,
     B11111111,
     B01111110,
     B00000000,
     B00000000},
  blink_it3_bmp []=
    {B00000000,
     B00000000,
     B00000000,
     B11111111,
     B11111111,
     B00000000,
     B00000000,
     B00000000};

boolean isPupileMoving = false;
int pupilePosition = 3;
int movePupileTo = 3;
int finalPupilePosition;

boolean isBlinking = false;
int blinkIteration = 0;
     
void setup() {
  Serial.begin(9600);
  
  matrix.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address

  matrix.clear();
  matrix.drawBitmap(0, 0, init_bmp, 8, 8, LED_GREEN);
  matrix2.clear();
  matrix2.drawBitmap(0, 0, init_bmp, 8, 8, LED_GREEN);

  clearPupiles(pupilePosition);
    
}

void paintPupiles(int pX){
  matrix.drawPixel(pX, 3, LED_GREEN);
  matrix.drawPixel(pX+1, 3, LED_GREEN);
  matrix.drawPixel(pX, 4, LED_GREEN);
  matrix.drawPixel(pX+1, 4, LED_GREEN);
  //matrix.writeDisplay();
  matrix2.drawPixel(pX, 3, LED_GREEN);
  matrix2.drawPixel(pX+1, 3, LED_GREEN);
  matrix2.drawPixel(pX, 4, LED_GREEN);
  matrix2.drawPixel(pX+1, 4, LED_GREEN);
  //matrix2.writeDisplay();
}

void clearPupiles(int pX){
  matrix.drawPixel(pX, 3, LOW);
  matrix.drawPixel(pX+1, 3, LOW);
  matrix.drawPixel(pX, 4, LOW);
  matrix.drawPixel(pX+1, 4, LOW);
  //matrix.writeDisplay();
  matrix2.drawPixel(pX, 3, LOW);
  matrix2.drawPixel(pX+1, 3, LOW);
  matrix2.drawPixel(pX, 4, LOW);
  matrix2.drawPixel(pX+1, 4, LOW);
  //matrix2.writeDisplay();   
}

void clearRows(int row){
  matrix.drawPixel(0, row, LOW);
  matrix.drawPixel(1, row, LOW);
  matrix.drawPixel(2, row, LOW);
  matrix.drawPixel(3, row, LOW);
  matrix.drawPixel(4, row, LOW);
  matrix.drawPixel(5, row, LOW);
  matrix.drawPixel(6, row, LOW);
  matrix.drawPixel(7, row, LOW);
  matrix2.drawPixel(0, row, LOW);
  matrix2.drawPixel(1, row, LOW);
  matrix2.drawPixel(2, row, LOW);
  matrix2.drawPixel(3, row, LOW);
  matrix2.drawPixel(4, row, LOW);
  matrix2.drawPixel(5, row, LOW);
  matrix2.drawPixel(6, row, LOW);
  matrix2.drawPixel(7, row, LOW);
}
    
void loop() {

  // Moving pupilles
  if ((random(0,60) == 1)&&!isPupileMoving){ // Calculate probability of moving puiles when resting.
    finalPupilePosition = random(1,6); // Final destination for the pupile.
    //Serial.println(movePupileTo);
    isPupileMoving = true;
  } else if (isPupileMoving) {    
    if ( pupilePosition < finalPupilePosition ){
      movePupileTo = pupilePosition+1;
    } else if ( pupilePosition > finalPupilePosition ){
      movePupileTo = pupilePosition-1; 
    } else { // Last iteration for moving pupiles
      isPupileMoving = false;
    }   
    paintPupiles(pupilePosition);
    clearPupiles(movePupileTo);
    pupilePosition = movePupileTo;
    
    matrix.writeDisplay();  
    matrix2.writeDisplay();   
  
  }

  // Blink
  if ((random(0,90) == 1)&&!isBlinking){ // Calculate probability of moving pupiles when resting.
    isBlinking = true;
    blinkIteration = 1;
    
  } else if (isBlinking) {    
    if (blinkIteration == 0) {
      isBlinking = false;
      matrix.clear();
      matrix2.clear();
      matrix.drawBitmap(0, 0, init_bmp, 8, 8, LED_GREEN);
      matrix2.drawBitmap(0, 0, init_bmp, 8, 8, LED_GREEN);
      clearPupiles(movePupileTo);
      
    } else if ((blinkIteration == 1) || (blinkIteration == 5)){
      matrix.clear();
      matrix2.clear();
      matrix.drawBitmap(0, 0, blink_it1_bmp, 8, 8, LED_GREEN);
      matrix2.drawBitmap(0, 0, blink_it1_bmp, 8, 8, LED_GREEN);
      clearPupiles(movePupileTo);
      if (blinkIteration == 1){ 
        blinkIteration = 2;
      } else {    
        blinkIteration = 0;
      }
      
    } else if ((blinkIteration == 2) || (blinkIteration == 4)){
      matrix.clear();
      matrix2.clear();
      matrix.drawBitmap(0, 0, blink_it2_bmp, 8, 8, LED_GREEN);
      matrix2.drawBitmap(0, 0, blink_it2_bmp, 8, 8, LED_GREEN);
      clearPupiles(movePupileTo);
      if (blinkIteration == 2){ 
        blinkIteration = 3;
      } else {        
        blinkIteration = 5;
      }
      
    } else if (blinkIteration == 3){
      matrix.clear();
      matrix2.clear();
      matrix.drawBitmap(0, 0, blink_it3_bmp, 8, 8, LED_GREEN);
      matrix2.drawBitmap(0, 0, blink_it3_bmp, 8, 8, LED_GREEN);
      clearPupiles(movePupileTo);
      blinkIteration = 4;
    }
    
    matrix.writeDisplay();  
    matrix2.writeDisplay();   
  }
  
  delay(40); // 1000ms/24 => aprox40 ms/frame
}
