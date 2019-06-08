/*
  Listener

  Having an LM3914 we want to know whether it's possible or not to know which output is HIGH and which is LOW.
  By rotating a 10k potentiometer leds lit as we raise voltage.

  Enrique Perez 31/03/2019

*/


int sensorPin1  = A1,
    sensorPin2  = A2,  
    sensorPin3  = A3,  
    sensorPin4  = A4,   
    sensorPin5  = A5,  
    sensorPin6  = A6,  
    sensorPin7  = A7,  
    sensorPin8  = A8,  
    sensorPin9  = A9,  
    sensorPin10 = A10,  
    sensorValue1 = 0,
    sensorValue2 = 0,  
    sensorValue3 = 0,  
    sensorValue4 = 0,  
    sensorValue5 = 0,  
    sensorValue6 = 0,  
    sensorValue7 = 0,  
    sensorValue8 = 0,  
    sensorValue9 = 0,  
    sensorValue10 = 0;  

 int percentage = 0; 
	

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(2, INPUT_PULLUP);    // sets the digital pin 2 as input

  Serial.begin(9600);
}

void loop() {


  // read the value from the sensor:
  sensorValue1 = analogRead(sensorPin1);
  sensorValue2 = analogRead(sensorPin2);
  sensorValue3 = analogRead(sensorPin3);
  sensorValue4 = analogRead(sensorPin4);
  sensorValue5 = analogRead(sensorPin5);
  sensorValue6 = analogRead(sensorPin6);
  sensorValue7 = analogRead(sensorPin7);
  sensorValue8 = analogRead(sensorPin8);
  sensorValue9 = analogRead(sensorPin9);
  sensorValue10 = analogRead(sensorPin10);

  if (sensorValue1 > 700)
	digitalWrite(LED_BUILTIN, LOW);
  else
  	digitalWrite(LED_BUILTIN, HIGH);

  if (sensorValue1 > 700) percentage = 10;
  else if (sensorValue2 > 700) percentage = 20;
  else if (sensorValue3 > 700) percentage = 30;
  else if (sensorValue4 > 700) percentage = 40;
  else if (sensorValue5 > 700) percentage = 50;
  else if (sensorValue6 > 700) percentage = 60;
  else if (sensorValue7 > 700) percentage = 70;
  else if (sensorValue8 > 700) percentage = 80;
  else if (sensorValue9 > 700) percentage = 90;
  else if (sensorValue10 > 700) percentage = 100;
  else percentage = 0;

  /*Serial.print("    1=");
  Serial.print(sensorValue1);
  Serial.print("    2=");
  Serial.print(sensorValue2);
  Serial.print("    3=");
  Serial.print(sensorValue3);
  Serial.print("    4=");
  Serial.print(sensorValue4);
  Serial.print("    5=");
  Serial.print(sensorValue5);
  Serial.print("    6=");
  Serial.print(sensorValue6);
  Serial.print("    7=");
  Serial.print(sensorValue7);
  Serial.print("    8=");
  Serial.print(sensorValue8);
  Serial.print("    9=");
  Serial.print(sensorValue9);
  Serial.print("    10=");
  Serial.print(sensorValue10);
  Serial.print("    percentage=");
  Serial.println(percentage);*/

  Serial.println(digitalRead(2));

  delay(1000);
}
/*
void setup() {
  pinMode(2, INPUT_PULLUP);    
  Serial.begin(9600);
}

void loop() {
  int sensorVal = digitalRead(2);
  //print out the value of the pushbutton
  Serial.println(sensorVal, DEC);
  delay(1000);            // waits for a second
}*/