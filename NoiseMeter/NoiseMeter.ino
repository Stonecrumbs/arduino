
/*  Program: NoiseMeter V0.1
 *   
 * 
 *   
 *  Goal: By using an audio sensor and two rotary encoders we can select 
 *        three zones being those low, medium and high. The boundary detween
 *        low and medium is selected by rotary encoder one. Rotary encoder 
 *        two is selecting the boundary between medium and high.
 *  
 *  
 *  v0.1 Interface trough 8x8 bicolor matrix implementation
 *  
 *  v0.2 implementng Eyes movement and effects
 *  
 *  Considerations: Min analog voltage read is 0 and max is 1024.
 *  
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrix2 = Adafruit_BicolorMatrix();

#define ORE_1_A  6  //output Rotary Encoder 1 0     (DT) 
#define ORE_1_B  7  //output Rotary Encoder 1 pin B (CLK)
#define ORE_1_SW 8  //Rotary Encoder 1 button.      (SW)
#define ORE_2_A  9  //output Rotary Encoder 1 pin A (DT)
#define ORE_2_B  10 //output Rotary Encoder 1 pin B (CLK)
 
int aVol;

static const int PROGMEM  analogPin = A0,// A0 on the sensor
                          interfaceDuration = 1000,
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

int eyesFrame;


int changeCode; /* This int will track what happened on each iteration 
                   0- Nothing happened
                   1- RE1 has been rotated clockwise. 
                   2- RE1 has been rotated counter clockwise.
                   3- RE2 has been rotated clockwise. 
                   4- RE2 has been rotated counter clockwise.
                */
int interfaceActive = 0; /* Indicates the number of milliseconds the interface will be actve. 
                                Once an action occured this variable is set to the munber of milliseconds 
                                we want the interface to be active. And will decrease every 
                                iteration until it reaches 0 and the eyes are shown up again. 
                              */

void setup() {
  pinMode (ORE_1_A, INPUT);
  pinMode (ORE_1_B, INPUT);
  pinMode (ORE_1_SW, INPUT);
  pinMode (ORE_2_A, INPUT);
  pinMode (ORE_2_B, INPUT);
  
  digitalWrite(ORE_1_SW, HIGH);
  
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
  matrix.drawBitmap(0, 0, init_bmp, 8, 8, LED_GREEN);
  matrix2.clear();
  matrix2.drawBitmap(0, 0, init_bmp, 8, 8, LED_GREEN);
  
  clearPupiles(pupilePosition);
}

void Print(){
  
    Serial.print(" RE_1_Percent: ");
    Serial.print(RE_1_Percent);
    Serial.print(" RE_2_Percent: ");
    Serial.println(RE_2_Percent);
    
} //Print


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
    Print();
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
    Print();
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

    //Print();
  }


  RE_1_Pos = (RE_1_Percent*1024.0)/100.0;
  RE_2_Pos = (RE_2_Percent*1024.0)/100.0;

  ORE_1_A_LastState = ORE_1_A_State;
  ORE_2_A_LastState = ORE_2_A_State;

}  //calculateREPositions


void CheckButtonPressed(){
  if (digitalRead(ORE_1_SW) == LOW){
    // initialise boundaries;
    RE_1_Percent = 20; 
    RE_2_Percent = 80; 
    Print();
  }
}


void zero(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX, 4, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX, 6, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX, 7, HIGH);
    matrix.drawPixel(iniX+1, 7, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX, 4, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX, 6, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX, 7, HIGH);
    matrix2.drawPixel(iniX+1, 7, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }  
}

void one(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  } 
  if (mat == 2){
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  } 
}

void two(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX, 6, HIGH);
    matrix.drawPixel(iniX, 7, HIGH);
    matrix.drawPixel(iniX+1, 7, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }  
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX, 6, HIGH);
    matrix2.drawPixel(iniX, 7, HIGH);
    matrix2.drawPixel(iniX+1, 7, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }  
}
void three(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX, 7, HIGH);
    matrix.drawPixel(iniX+1, 7, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX, 7, HIGH);
    matrix2.drawPixel(iniX+1, 7, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void four(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX, 4, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX, 4, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void five(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX, 7, HIGH);
    matrix.drawPixel(iniX+1, 7, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX, 7, HIGH);
    matrix2.drawPixel(iniX+1, 7, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void six(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX, 6, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX, 7, HIGH);
    matrix.drawPixel(iniX+1, 7, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX, 6, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX, 7, HIGH);
    matrix2.drawPixel(iniX+1, 7, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void seven(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void eight(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX, 4, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX, 6, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX, 7, HIGH);
    matrix.drawPixel(iniX+1, 7, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX, 4, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX, 6, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX, 7, HIGH);
    matrix2.drawPixel(iniX+1, 7, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void nine(int iniX, int mat){
  if (mat == 1){
    matrix.drawPixel(iniX, 3, HIGH);
    matrix.drawPixel(iniX+1, 3, HIGH);
    matrix.drawPixel(iniX+2, 3, HIGH);
    matrix.drawPixel(iniX, 4, HIGH);
    matrix.drawPixel(iniX+2, 4, HIGH);
    matrix.drawPixel(iniX, 5, HIGH);
    matrix.drawPixel(iniX+1, 5, HIGH);
    matrix.drawPixel(iniX+2, 5, HIGH);
    matrix.drawPixel(iniX+2, 6, HIGH);
    matrix.drawPixel(iniX+2, 7, HIGH);
  }    
  if (mat == 2){
    matrix2.drawPixel(iniX, 3, HIGH);
    matrix2.drawPixel(iniX+1, 3, HIGH);
    matrix2.drawPixel(iniX+2, 3, HIGH);
    matrix2.drawPixel(iniX, 4, HIGH);
    matrix2.drawPixel(iniX+2, 4, HIGH);
    matrix2.drawPixel(iniX, 5, HIGH);
    matrix2.drawPixel(iniX+1, 5, HIGH);
    matrix2.drawPixel(iniX+2, 5, HIGH);
    matrix2.drawPixel(iniX+2, 6, HIGH);
    matrix2.drawPixel(iniX+2, 7, HIGH);
  }    
}

void drawBar(){

  int b1 = (int)((RE_1_Percent*16)/100);
  int b2 = (int)((RE_2_Percent*16)/100);   

  for (int i=0;i<8;i++){
    if (i<b1) matrix2.drawPixel(i, 0, LED_GREEN);
    else if (i>b2) matrix2.drawPixel(i, 0, LED_RED);
         else matrix2.drawPixel(i, 0, LED_YELLOW);    
  }
  for (int i=8;i<16;i++){
    if (i<b1) matrix.drawPixel(i-8, 0, LED_GREEN);
    else if (i>b2) matrix.drawPixel(i-8, 0, LED_RED);
         else matrix.drawPixel(i-8, 0, LED_YELLOW);    
  }
}

void drawNumbers(){
  //d1 first digit RE_1_Percent
  int d1 = RE_1_Percent/10;
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
  int d2 = RE_1_Percent-(d1*10);
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
  d1 = RE_2_Percent/10;
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
  d2 = RE_2_Percent-(d1*10);
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

void drawInterface(){
  matrix.clear();
  matrix2.clear();
  
  drawBar();
  drawNumbers();
  
  matrix.writeDisplay();   
  matrix2.writeDisplay();   
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

  changeCode = 0;
  
  //Read the audio analog voltage value 
  aVol = analogRead(analogPin);

  CheckButtonPressed();

  CalculateREPositions();

  //Calculate the vol for % defined in RE1 and RE2.
  if (changeCode > 0) {
    interfaceActive = interfaceDuration;

    Serial.print(" Pos 1 = ");Serial.print(RE_1_Pos);
    Serial.print(" Pos 2 = ");Serial.println(RE_2_Pos);
    
  }
  if (interfaceActive > 0) {
    drawInterface();

    interfaceActive --;
    
  } else {

    if (eyesFrame == 0){
      
      eyesFrame = eyesVelocity;
      
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
    } else {
      eyesFrame --;
    }
  }


  //delay(40); // 1000ms/24 => aprox40 ms/frame
  
}
