 /* 
This program draws an eye on MAX7219 which pupile can be controlled by moving a joystick

*/


//We always have to include the library
#include "LedControl.h"
#include "math.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.

This is for the Joystick
 A0 connected to vrX
 A1 connected to vrY
 pin2 connected to SW (NOT USED IN THIS PROGRAM) 
 */
LedControl lc=LedControl(12,11,10,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=100;


// Analog Joystick vars and consts
// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

int posX = 3;
int posY = 4;
int posX_prev = 3;
int posY_prev = 4;


void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,1);
  /* and clear the display */
  lc.clearDisplay(0);

  // Initialise joystick
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  Serial.begin(9600);
}


void writeEye() {
  byte eyeBall[8]={B00111100,B01111110,B11111111,B11111111,B11111111,B11111111,B01111110,B00111100};
  lc.setRow(0,0,eyeBall[0]);
  lc.setRow(0,1,eyeBall[1]);
  lc.setRow(0,2,eyeBall[2]);
  lc.setRow(0,3,eyeBall[3]);
  lc.setRow(0,4,eyeBall[4]);
  lc.setRow(0,5,eyeBall[5]);
  lc.setRow(0,6,eyeBall[6]);
  lc.setRow(0,7,eyeBall[7]);
  
}

boolean inCorners(int y, int x){
  if ( (y==0 and x==0) or
       (y==1 and x==0) or
       (y==0 and x==1) or
       
       (y==7 and x==0) or
       (y==6 and x==0) or
       (y==7 and x==1) or
       
       (y==7 and x==6) or
       (y==7 and x==7) or
       (y==6 and x==7) or
       
       (y==0 and x==6) or
       (y==0 and x==7) or
       (y==1 and x==7) 
     ){
      return true;
     }
  else return false;
}

void unlitCorners(){
  
  lc.setLed(0,0,0,false);
  lc.setLed(0,0,1,false);
  lc.setLed(0,1,0,false);
  
  lc.setLed(0,7,0,false);
  lc.setLed(0,6,0,false);
  lc.setLed(0,7,1,false);
  
  lc.setLed(0,7,6,false);
  lc.setLed(0,7,7,false);
  lc.setLed(0,6,7,false);
  
  lc.setLed(0,0,6,false);
  lc.setLed(0,0,7,false);
  lc.setLed(0,1,7,false);
}

void loop() { 
  
  writeEye();
      
  while (true) {
    posX = round( analogRead(X_pin) / 146.142);
    posY = round( analogRead(Y_pin) / 146.142);

    
    if ((posX != posX_prev or posY != posY_prev) 
        and !inCorners(posY,posX)
        and !inCorners(posY_prev,posX_prev)
       ){
        
      /*    
      Serial.print("PosX ");
      Serial.print(posX);
      Serial.print("\n");  
      Serial.print("PosY ");
      Serial.print(posY);
      Serial.print("\n");*/
    
      lc.setLed(0,posY_prev,posX_prev,true);
      lc.setLed(0,posY,posX,false);
    }
  
    delay(100);
  
    posX_prev = posX;
    posY_prev = posY;

    unlitCorners();
    
  }
}
