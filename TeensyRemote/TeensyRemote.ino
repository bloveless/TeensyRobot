#include <IRremote.h>

int xPin = 17;
int yPin = 16;
int xOffset = 0;
int yOffset = 0;
int xVal = 0;
int yVal = 0;

IRsend irsend;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  
  readJoystick();
  xOffset = xVal * -1;
  yOffset = yVal * -1;
}

void loop() {
  readJoystick();
  irsend.sendSony(((xVal + 100) * 1000) + (yVal + 100), 20);
  delay(5d0);
}

void readJoystick() {
  xVal = map(analogRead(xPin), 0, 1023, -100, 100) + xOffset;
  if(xVal > 100) xVal = 100;
  else if(xVal < -100) xVal = -100;
  else if(xVal < 5 && xVal > -5) xVal = 0;
  yVal = map(analogRead(yPin), 61, 1023, -100, 100) + yOffset;
  if(yVal > 100) yVal = 100;
  else if(yVal < -100) yVal = -100;
  else if(yVal < 5 && yVal > -5) yVal = 0;
}

int countLength(int n) {
  return floor(log10(abs(n))) + 1;
}
