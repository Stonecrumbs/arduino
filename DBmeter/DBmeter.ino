


const PROGMEM int ledPin = 5;
const PROGMEM int digitalPin = 3;
const PROGMEM int analogPin = A0;

const int delayTime = 1000;

unsigned long previousMillis = 0;
bool ledState = LOW;

void setup(){
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(digitalPin, INPUT);
}

void checkAnalog(){
  static int maxA = 0;
  static int minA = 1023;
  int aValue = 0;
  float volts;
  float maxVolts;
  float minVolts;

  aValue = analogRead(analogPin);

  if (aValue < minA) {minA = aValue;}
  if (aValue > maxA) {maxA = aValue;}

  volts = (aValue*5.0)/1024.0;
  minVolts = (minA*5.0)/1024.0;
  maxVolts = (maxA*5.0)/1024.0;

  Serial.print("Analog Value = ");Serial.println(aValue);//, DEC);
/*  Serial.print(" Max = ");Serial.print(maxA);
  Serial.print(" Min = ");Serial.print(minA);
  Serial.print(" minVolts = ");Serial.print(minVolts);
  Serial.print(" maxVolts = ");Serial.print (maxVolts);
  Serial.print(" volts = ");Serial.println(volts);*/
  
   //PRINT INFO FUNCTION HERE

}

void checkDigital(){
  bool dValue = HIGH;
  unsigned long currentMillis = millis();

  dValue = digitalRead(digitalPin);

  if (((currentMillis - previousMillis) > delayTime) && (dValue == LOW)) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    previousMillis = currentMillis;

  }
    
  //Serial.print("Digital value = ");Serial.println(dValue);
  
}


void loop(){

  checkAnalog();
  checkDigital();
  
}
