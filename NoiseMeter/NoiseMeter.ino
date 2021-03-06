/*  Program: NoiseMeter V0.10
 *   
 * 
 *   
 *  Goal: By using an audio sensor and two rotary encoders we can select 
 *        three zones being those low, medium and high. The boundary detween
 *        low and medium is selected by rotary encoder one. Rotary encoder 
 *        two is selecting the boundary between medium and high.
 *  
 *  v0.1 Interface trough 8x8 bieyesColor matrix implementation
 *  
 *  v0.2 Implementng Eyes movement and effects
 *  
 *  v0.3 Implementng Audiometer bar
 *  
 *  v0.4 Interface controller is no longer active for a period of time. It will be toggled 
 *       in and out by using ER buttons.
 *       
 *  v0.5 Implementing eyesColor depending on the audio analog lecture
 *       1 = Red
 *       2 = Yellow
 *       3 = Green
 *       
 *  v0.8 Exponetial filtring on analog reading to stabilize values. Too much noise
 *       detected. Filter.h added.
 *       
 *  v0.9 Now by clicking ER1 the actual analog read from the sound sensor is shown.
 *       This will help to check, test and troubleshoot the noise problem on readings.
 *       
 *  v0.10 Exponetial filtering is not as stable as we want. We try Accumulatice average
 *  
 *  Considerations: Min analog voltage read is 0 and max is 1024.
 *  
 */

#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"
  
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();

#define ORE_1_A  6  //output Rotary Encoder 1 0     (DT) 
#define ORE_1_B  7  //output Rotary Encoder 1 pin B (CLK)
#define ORE_1_SW 8  //Rotary Encoder 1 button.      (SW)
#define ORE_2_A  9  //output Rotary Encoder 1 pin A (DT)
#define ORE_2_B  10 //output Rotary Encoder 1 pin B (CLK)
#define ORE_2_SW 11  //Rotary Encoder 2 button.      (SW)
 
int aVolRead=0, aVolAvg=0, aVolMaxVal=400, aVolCount=0, aVolSum=0;

  
static const int PROGMEM  analogPin = A1,// A0 on the sensor
                          eyesVelocity = 160; 
              
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

int RE_1_Percent = 20; //Rotary Encoder 1 Percentage
int RE_1_Pos;
int ORE_1_A_State;
int ORE_1_A_LastState;

int RE_2_Percent = 80; //Rotary Encoder 2 Percentage
int RE_2_Pos;
int ORE_2_A_State;
int ORE_2_A_LastState;


boolean isPupileMoving = false;
int pupilePosition = 3;
int movePupileTo = 3;
int finalPupilePosition;

boolean isBlinking = false;
int blinkIteration = 0;

int eyesFrameControl;


int changeCode; /* This int will track what happened on each iteration 
                   0- Nothing happened
                   1- RE1 has been rotated clockwise. 
                   2- RE1 has been rotated counter clockwise.
                   3- RE2 has been rotated clockwise. 
                   4- RE2 has been rotated counter clockwise.
                */
                
int interfaceToggle = 0; // 0 means interface off, 1 means on

int eyesColor;
                          

void setup() {
  pinMode (ORE_1_A, INPUT);
  pinMode (ORE_1_B, INPUT);
  pinMode (ORE_1_SW, INPUT);
  pinMode (ORE_2_A, INPUT);
  pinMode (ORE_2_B, INPUT);
  pinMode (ORE_2_SW, INPUT);
  
  digitalWrite(ORE_1_SW, HIGH);
  digitalWrite(ORE_2_SW, HIGH);
  
  Serial.begin(9600);
  
  ORE_1_A_State = digitalRead(ORE_1_A);
  ORE_1_A_LastState = digitalRead(ORE_1_A);
  ORE_2_A_State = digitalRead(ORE_1_A);
  ORE_2_A_LastState = digitalRead(ORE_1_A);

    
  matrix.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address
  
  initEye(); 
  
}

void initEye(){
  matrix.clear();
  matrix.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
  matrix2.clear();
  matrix2.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
  
  clearPupiles(pupilePosition);
}

void CalculateREPositions(){

  // Calculate percentage selected fot Rotary Encoder 1
  ORE_1_A_State = digitalRead(ORE_1_A);
  if (ORE_1_A_State != ORE_1_A_LastState){
    if (digitalRead(ORE_1_B) != ORE_1_A_State){
      if (RE_1_Percent < 99) { 
        RE_1_Percent ++;
        changeCode = 1;
      }
    } else {
      if (RE_1_Percent > 0){
        RE_1_Percent --;
        changeCode = 2;
      }
    }
  }

  
  // Calculate percentage selected fot Rotary Encoder 2
  ORE_2_A_State = digitalRead(ORE_2_A);
  if (ORE_2_A_State != ORE_2_A_LastState){
    if (digitalRead(ORE_2_B) != ORE_2_A_State){
      if (RE_2_Percent < 100) { 
        RE_2_Percent ++;
        changeCode = 3;
      }
    } else {
      if (RE_2_Percent > 1){
        RE_2_Percent --;
        changeCode = 4;
      }
    }
  }


  //RE1 can't be higher than RE2
  if (RE_2_Percent < RE_1_Percent){
    //wich one is moving 
    if (changeCode == 1 || changeCode == 2){
      RE_2_Percent = RE_1_Percent+1;
    } 
    if (changeCode == 3 || changeCode == 4){
      RE_1_Percent = RE_2_Percent-1;
    } 
  }


  RE_1_Pos = (RE_1_Percent*1024.0)/100.0;
  RE_2_Pos = (RE_2_Percent*1024.0)/100.0;

  ORE_1_A_LastState = ORE_1_A_State;
  ORE_2_A_LastState = ORE_2_A_State;

}  //calculateREPositions

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

void checkColor(float volume){
  
    // analog read percent
  int aVol_Percent = (int)((volume*100.0)/aVolMaxVal);

  if (aVol_Percent<RE_1_Percent) eyesColor = 3; //Green
  else if (aVol_Percent>RE_2_Percent) eyesColor = 1; //Red
  else  eyesColor = 2; //Yellow
}

void drawBar(){

  // boundaries
  int b1 = (int)((RE_1_Percent*15.0)/100.0);
  int b2 = (int)((RE_2_Percent*15.0)/100.0);   
  
  // Meter
  int m1 = (int)((aVolAvg*15.0)/aVolMaxVal);
  
  for (int i=0;i<8;i++){
    if (i<b1) {
      matrix2.drawPixel(i, 0, 3);
      if (i<m1) matrix2.drawPixel(i, 1, 3);  
    }
    else if (i>b2) {
      matrix2.drawPixel(i, 0, 1);
      if (i<m1) matrix2.drawPixel(i, 1, 1);  
    }
    else {
      matrix2.drawPixel(i, 0, 2);
      if (i<m1) matrix2.drawPixel(i, 1, 2);  
    }           
  }
  
  for (int i=8;i<16;i++){
    if (i<b1) {
      matrix.drawPixel(i-8, 0, 3);
      if (i<m1) matrix.drawPixel(i-8, 1, 3); 
    }
    else if (i>b2){
      matrix.drawPixel(i-8, 0, 1);
      if (i<m1) matrix.drawPixel(i-8, 1, 1);  
    }
    else {
      matrix.drawPixel(i-8, 0, 2);   
      if (i<m1) matrix.drawPixel(i-8, 1, 2);  
    }
  }
  
}

void drawNumbers(){
  //d1 first digit RE_1_Percent
  int d1 = RE_1_Percent/10.0;
  if      (d1 == 0)  zero(1,2);
  else if (d1 == 1)   one(1,2);
  else if (d1 == 2)   two(1,2);
  else if (d1 == 3) three(1,2);
  else if (d1 == 4)  four(1,2);
  else if (d1 == 5)  five(1,2);
  else if (d1 == 6)   six(1,2);
  else if (d1 == 7) seven(1,2);
  else if (d1 == 8) eight(1,2);
  else if (d1 == 9)  nine(1,2);
  //d2 second digit RE_1_Percent
  int d2 = RE_1_Percent-(d1*10.0);
  if      (d2 == 0)  zero(5,2);
  else if (d2 == 1)   one(5,2);
  else if (d2 == 2)   two(5,2);
  else if (d2 == 3) three(5,2);
  else if (d2 == 4)  four(5,2);
  else if (d2 == 5)  five(5,2);
  else if (d2 == 6)   six(5,2);
  else if (d2 == 7) seven(5,2);
  else if (d2 == 8) eight(5,2);
  else if (d2 == 9)  nine(5,2);

  //d1 first digit RE_2_Percent
  d1 = RE_2_Percent/10.0;
  if      (d1 == 0)  zero(1,1);
  else if (d1 == 1)   one(1,1);
  else if (d1 == 2)   two(1,1);
  else if (d1 == 3) three(1,1);
  else if (d1 == 4)  four(1,1);
  else if (d1 == 5)  five(1,1);
  else if (d1 == 6)   six(1,1);
  else if (d1 == 7) seven(1,1);
  else if (d1 == 8) eight(1,1);
  else if (d1 == 9)  nine(1,1);
  //d2 second digit RE_2_Percent
  d2 = RE_2_Percent-(d1*10.0);
  if      (d2 == 0)  zero(5,1);
  else if (d2 == 1)   one(5,1);
  else if (d2 == 2)   two(5,1);
  else if (d2 == 3) three(5,1);
  else if (d2 == 4)  four(5,1);
  else if (d2 == 5)  five(5,1);
  else if (d2 == 6)   six(5,1);
  else if (d2 == 7) seven(5,1);
  else if (d2 == 8) eight(5,1);
  else if (d2 == 9)  nine(5,1);

}

void drawRead(float num){
  //d1 thousands
  int d1 = num/1000.0;
  if      (d1 == 0)  zero(1,2);
  else if (d1 == 1)   one(1,2);
  else if (d1 == 2)   two(1,2);
  else if (d1 == 3) three(1,2);
  else if (d1 == 4)  four(1,2);
  else if (d1 == 5)  five(1,2);
  else if (d1 == 6)   six(1,2);
  else if (d1 == 7) seven(1,2);
  else if (d1 == 8) eight(1,2);
  else if (d1 == 9)  nine(1,2);
  //d2 hundreds
  int d2 = (num-(d1*1000.0))/100.0;
  if      (d2 == 0)  zero(5,2);
  else if (d2 == 1)   one(5,2);
  else if (d2 == 2)   two(5,2);
  else if (d2 == 3) three(5,2);
  else if (d2 == 4)  four(5,2);
  else if (d2 == 5)  five(5,2);
  else if (d2 == 6)   six(5,2);
  else if (d2 == 7) seven(5,2);
  else if (d2 == 8) eight(5,2);
  else if (d2 == 9)  nine(5,2);  
  //d3 tens
  int d3 = (num-(d1*1000.0)-(d2*100.0))/10.0;
  if      (d3 == 0)  zero(1,1);
  else if (d3 == 1)   one(1,1);
  else if (d3 == 2)   two(1,1);
  else if (d3 == 3) three(1,1);
  else if (d3 == 4)  four(1,1);
  else if (d3 == 5)  five(1,1);
  else if (d3 == 6)   six(1,1);
  else if (d3 == 7) seven(1,1);
  else if (d3 == 8) eight(1,1);
  else if (d3 == 9)  nine(1,1);
  //d4 ones
  int d4 = num-(d1*1000.0)-(d2*100.0)-(d3*10.0);
  if      (d4 == 0)  zero(5,1);
  else if (d4 == 1)   one(5,1);
  else if (d4 == 2)   two(5,1);
  else if (d4 == 3) three(5,1);
  else if (d4 == 4)  four(5,1);
  else if (d4 == 5)  five(5,1);
  else if (d4 == 6)   six(5,1);
  else if (d4 == 7) seven(5,1);
  else if (d4 == 8) eight(5,1);
  else if (d4 == 9)  nine(5,1);
}

void drawInterface(){
  matrix.clear();
  matrix2.clear();
  
  drawBar();
  //drawMeter();
  drawNumbers();
  
  matrix.writeDisplay();   
  matrix2.writeDisplay();   
}


void paintPupiles(int pX){
  matrix.drawPixel(pX, 3, eyesColor);
  matrix.drawPixel(pX+1, 3, eyesColor);
  matrix.drawPixel(pX, 4, eyesColor);
  matrix.drawPixel(pX+1, 4, eyesColor);
  //matrix.writeDisplay();
  matrix2.drawPixel(pX, 3, eyesColor);
  matrix2.drawPixel(pX+1, 3, eyesColor);
  matrix2.drawPixel(pX, 4, eyesColor);
  matrix2.drawPixel(pX+1, 4, eyesColor);
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
    
void CheckButtonPressed(){
  
  if (digitalRead(ORE_1_SW) == LOW){
    /*
    // initialise boundaries;
    RE_1_Percent = 20; 
    RE_2_Percent = 80; 
    */
    
    while (digitalRead(ORE_1_SW) == LOW){
      true;
    }
    if (interfaceToggle == 0) interfaceToggle = 1; 
    else  if (interfaceToggle == 1) interfaceToggle = 0;
  
  }

  if (digitalRead(ORE_2_SW) == LOW){  
    matrix.clear();
    matrix2.clear();
  
    drawRead(aVolAvg);
  
    matrix.writeDisplay();   
    matrix2.writeDisplay();   
  }
  
}


void loop() {

  changeCode = 0;
  
  //Read the audio analog voltage value
  aVolRead = analogRead(analogPin);
  if (aVolCount < 64){
    aVolCount++;
    aVolSum = aVolSum + aVolRead;
  } else if (aVolCount == 64){
    aVolAvg = aVolSum / 64;
    aVolCount = 65;
  }
  aVolAvg = (((aVolAvg<<6)-aVolAvg)+aVolRead)>>6;
  
  //Serial.println(aVolAvg);
    
  checkColor(aVolAvg);

  CheckButtonPressed();

  CalculateREPositions();

  if (interfaceToggle > 0) {
    drawInterface();
    
  } else {

    if (eyesFrameControl == 0){
      initEye();
      eyesFrameControl = eyesVelocity;
      
      // Moving pupilles
      if ((random(0,60) == 1)&&!isPupileMoving){ // Calculate probability of moving puiles when resting.
        finalPupilePosition = random(1,6); // Final destination for the pupile.
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
          matrix.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, init_bmp, 8, 8, eyesColor);
          clearPupiles(movePupileTo);
          
        } else if ((blinkIteration == 1) || (blinkIteration == 5)){
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, blink_it1_bmp, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, blink_it1_bmp, 8, 8, eyesColor);
          clearPupiles(movePupileTo);
          if (blinkIteration == 1){ 
            blinkIteration = 2;
          } else {    
            blinkIteration = 0;
          }
          
        } else if ((blinkIteration == 2) || (blinkIteration == 4)){
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, blink_it2_bmp, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, blink_it2_bmp, 8, 8, eyesColor);
          clearPupiles(movePupileTo);
          if (blinkIteration == 2){ 
            blinkIteration = 3;
          } else {        
            blinkIteration = 5;
          }
          
        } else if (blinkIteration == 3){
          matrix.clear();
          matrix2.clear();
          matrix.drawBitmap(0, 0, blink_it3_bmp, 8, 8, eyesColor);
          matrix2.drawBitmap(0, 0, blink_it3_bmp, 8, 8, eyesColor);
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
  
}
