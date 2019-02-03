
/*  Program: NoiseMeter
 *   
 * 
 *   
 *  Goal: By using an audio sensor and two rotary encoders we can select 
 *        three zones being those low, medium and high. The boundary detween
 *        low and medium is selected by rotary encoder one. Rotary encoder 
 *        two is selecting the boundary between medium and high.
 *  
 *  
 *  Considerations: Min analog voltage read is 0 and max is 1024.
 *  
 */

#define ORE_1_A  6  //output Rotary Encoder 1 0     (DT) 
#define ORE_1_B  7  //output Rotary Encoder 1 pin B (CLK)
#define ORE_1_SW 8  //Rotary Encoder 1 button.      (SW)
#define ORE_2_A  9  //output Rotary Encoder 1 pin A (DT)
#define ORE_2_B  10 //output Rotary Encoder 1 pin B (CLK)
 
int aVol;

const PROGMEM int analogPin = A0; // A0 on the sensor

int RE_1_Percent = 20; //Rotary Encoder 1 Percentage
int RE_1_Pos;
int ORE_1_A_State;
int ORE_1_A_LastState;

int RE_2_Percent = 80; //Rotary Encoder 2 Percentage
int RE_2_Pos;
int ORE_2_A_State;
int ORE_2_A_LastState;

int changeCode; /* This int will track what happened on each iteration 
                   0- Nothing happened
                   1- RE1 has been rotated clockwise. 
                   2- RE1 has been rotated counter clockwise.
                   3- RE2 has been rotated clockwise. 
                   4- RE2 has been rotated counter clockwise.
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


void loop() {

  changeCode = 0;
  
  //Read the audio analog voltage value 
  aVol = analogRead(analogPin);

  CheckButtonPressed();

  CalculateREPositions();

  //Calculate the vol for % defined in RE1 and RE2.
  if (changeCode > 0) {

    Serial.print(" Pos 1 = ");Serial.print(RE_1_Pos);
    Serial.print(" Pos 2 = ");Serial.println(RE_2_Pos);

    // Interfaz de control a traves de matrices de led
    
  } else {

    // Pintar y animar ojos.

  }

  
  
}
