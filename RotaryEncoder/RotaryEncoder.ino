/* Arduino rotary encoder */

#define outputA 9
#define outputB 10 

int counter = 0;
int aState;
int aLastState;

void setup(){
  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);

  Serial.begin (9600);

  aLastState = digitalRead(outputA);
}

void loop(){
  aState = digitalRead(outputA);
  if (aState != aLastState){
    if (digitalRead(outputB) != aState){
      counter ++;
    } else {
      counter --;
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState;
}

 
