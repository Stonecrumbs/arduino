#include <SPI.h>
#include <Wire.h>

//BlinkingEyes.ino

#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"


Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();


static const int PROGMEM  eyesVelocity = 16000; 

// Note these bitmaps don't take the pupiles into account. Pupiles will be drawn (set to LOW) when needed.
static const uint8_t PROGMEM  init_bmp []={B00011000,B01111110,B11111111,B11111111,B11111111,B11111111,B01111110,B00011000},
							  blink_bmp1 []={B00000000,B00011000,B01111110,B11111111,B11111111,B11111111,B01111110,B00011000},
							  blink_bmp2 []={B00000000,B00000000,B00111100,B11111111,B11111111,B01111110,B00000000,B00000000},
							  blink_bmp3 []={B00000000,B00000000,B00000000,B11111111,B11111111,B00000000,B00000000,B00000000};


boolean isPupileMoving = false;
int pupilePosition = 3;
int movePupileTo = 3;
int finalPupilePosition;

boolean isBlinking = false;
int blinkIteration = 0;

int eyesFrameControl = 0;
int eyesColor = 3; // 1 red, 2 Yellow, 3 Green


double probabilityOfBlinking = 60;
double probabilityOfMovingPupiles = 90;

void setup() {
  
  // Read documentation about how to set the addresses on IC2 bus.
  matrix.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address

}



void initEye(){
  // Basically draws an ellipse. 
  matrix.clear();
  matrix.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
  matrix2.clear();
  matrix2.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
  
  clearPupiles(pupilePosition);
}

void zero(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX, 4, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX, 6, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX, 7, eyesColor);
    matrix.drawPixel(iniX+1, 7, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX, 4, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX, 7, eyesColor);
    matrix2.drawPixel(iniX+1, 7, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }  
}

void one(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  } 
  if (mat == 2){
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  } 
}

void two(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX, 6, eyesColor);
    matrix.drawPixel(iniX, 7, eyesColor);
    matrix.drawPixel(iniX+1, 7, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }  
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX, 6, eyesColor);
    matrix2.drawPixel(iniX, 7, eyesColor);
    matrix2.drawPixel(iniX+1, 7, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }  
}
void three(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX, 7, eyesColor);
    matrix.drawPixel(iniX+1, 7, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX, 7, eyesColor);
    matrix2.drawPixel(iniX+1, 7, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void four(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX, 4, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX, 4, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void five(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX, 7, eyesColor);
    matrix.drawPixel(iniX+1, 7, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX, 7, eyesColor);
    matrix2.drawPixel(iniX+1, 7, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void six(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX, 6, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX, 7, eyesColor);
    matrix.drawPixel(iniX+1, 7, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX, 7, eyesColor);
    matrix2.drawPixel(iniX+1, 7, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void seven(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void eight(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX, 4, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX, 6, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX, 7, eyesColor);
    matrix.drawPixel(iniX+1, 7, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX, 4, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX, 7, eyesColor);
    matrix2.drawPixel(iniX+1, 7, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void nine(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, eyesColor);
    matrix.drawPixel(iniX+1, 3, eyesColor);
    matrix.drawPixel(iniX+2, 3, eyesColor);
    matrix.drawPixel(iniX, 4, eyesColor);
    matrix.drawPixel(iniX+2, 4, eyesColor);
    matrix.drawPixel(iniX, 5, eyesColor);
    matrix.drawPixel(iniX+1, 5, eyesColor);
    matrix.drawPixel(iniX+2, 5, eyesColor);
    matrix.drawPixel(iniX+2, 6, eyesColor);
    matrix.drawPixel(iniX+2, 7, eyesColor);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, eyesColor);
    matrix2.drawPixel(iniX+1, 3, eyesColor);
    matrix2.drawPixel(iniX+2, 3, eyesColor);
    matrix2.drawPixel(iniX, 4, eyesColor);
    matrix2.drawPixel(iniX+2, 4, eyesColor);
    matrix2.drawPixel(iniX, 5, eyesColor);
    matrix2.drawPixel(iniX+1, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 5, eyesColor);
    matrix2.drawPixel(iniX+2, 6, eyesColor);
    matrix2.drawPixel(iniX+2, 7, eyesColor);
  }    
}

void paintPupiles(int pX){
  matrix.drawPixel(pX, 3, eyesColor);
  matrix.drawPixel(pX+1, 3, eyesColor);
  matrix.drawPixel(pX, 4, eyesColor);
  matrix.drawPixel(pX+1, 4, eyesColor);

  matrix2.drawPixel(pX, 3, eyesColor);
  matrix2.drawPixel(pX+1, 3, eyesColor);
  matrix2.drawPixel(pX, 4, eyesColor);
  matrix2.drawPixel(pX+1, 4, eyesColor);
}

void clearPupiles(int pX){
  matrix.drawPixel(pX, 3, LOW);
  matrix.drawPixel(pX+1, 3, LOW);
  matrix.drawPixel(pX, 4, LOW);
  matrix.drawPixel(pX+1, 4, LOW);

  matrix2.drawPixel(pX, 3, LOW);
  matrix2.drawPixel(pX+1, 3, LOW);
  matrix2.drawPixel(pX, 4, LOW);
  matrix2.drawPixel(pX+1, 4, LOW);
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


	/*
		The movement of the eye happens every eyesVelocity times, when eyesFrameControl reaches 0. 
		eyesFrameControl is decreasing by one on every iteration. 
	*/
    if (eyesFrameControl == 0){ // 

      //8x8 BiColor Matrix need to be initialised this sets both matrix to init bitmap.	
      initEye();

      /* eyesVelocity is the number of loops that will be skipped between each frame. 
                      This number may vary depending on the quantity of actions in the main loop. 
                      The lower eyesvelocity is, the fastest they move. 
      */
      eyesFrameControl = eyesVelocity;
      
      // Moving pupilles
      if ((random(0,probabilityOfMovingPupiles) == 1)&&!isPupileMoving){ // Calculate probability of moving puiles when they're resting.
        finalPupilePosition = random(1,6); // Final random destination for the pupile. 6 makes it to never reach the edges.
        isPupileMoving = true;
      } else if (isPupileMoving) {    
      	// calculate direction, left or right. We need to know where they are and where they will be next to create the movement.
      	// note that we're actually setting LOW the leds to draw the pipile.
        if ( pupilePosition < finalPupilePosition ){
          movePupileTo = pupilePosition+1;
        } else if ( pupilePosition > finalPupilePosition ){
          movePupileTo = pupilePosition-1; 
        } else { // Last iteration for moving pupiles or pupilePosition = finalPupilePosition
          isPupileMoving = false;
        }   
        paintPupiles(pupilePosition); // Set HIGH the leds where pipiles were the previous frame to erase them.
        clearPupiles(movePupileTo);   // Set LOW the leds where the position has been calculated in this frame.
        pupilePosition = movePupileTo;// prepare next movement
        
        matrix.writeDisplay();  
        matrix2.writeDisplay();   
      
      }
    
      // Blink
      /*
      	If probability engages the blink action, there will be a set of 3 frames painting half of the blink forward. Then it goes backwards the other half until it reaches init state.
      */
      if ((random(0,probabilityOfBlinking) == 1)&&!isBlinking){ // Calculate probability of blinking when they're resting.
        isBlinking = true;
        blinkIteration = 1;
        
      } else if (isBlinking) {    
        if (blinkIteration == 0) {
          isBlinking = false;
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
          clearPupiles(movePupileTo); // after clearing we need to paint the pupiles on its place again. They can be even moving while blinking.
          
        } else if ((blinkIteration == 1) || (blinkIteration == 5)){
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, blink_bmp1, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, blink_bmp1, 8, 8, eyesColor);
          clearPupiles(movePupileTo);
          if (blinkIteration == 1){ 
            blinkIteration = 2;
          } else {    
            blinkIteration = 0;
          }
          
        } else if ((blinkIteration == 2) || (blinkIteration == 4)){
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, blink_bmp2, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, blink_bmp2, 8, 8, eyesColor);
          clearPupiles(movePupileTo);
          if (blinkIteration == 2){ 
            blinkIteration = 3;
          } else {        
            blinkIteration = 5;
          }
          
        } else if (blinkIteration == 3){
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, blink_bmp3, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, blink_bmp3, 8, 8, eyesColor);
          clearPupiles(movePupileTo);
          blinkIteration = 4;
        }
  
      matrix.writeDisplay();  
      matrix2.writeDisplay();   
      }
    } else {
      eyesFrameControl --;
    }
}
