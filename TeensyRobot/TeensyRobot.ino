#include <Servo.h>
#include "TrackSpeed.h"

int ledPin =          13;
int buttonPin =       14;
int enable12Pin =     2;
int driver1Pin =      3;
int driver2Pin =      4;
int enable34Pin =     20;
int driver3Pin =      21;
int driver4Pin =      22;

int proximityPin =    15;

int armPin =          25;

int leftSide =        1;
int rightSide =       1;

char incomingChar =   0; // for reading and storing serial data

int prevButtonState = 0;

int useArms =         0;
int armPosition =     0;
Servo armServo;

int autoMode =        0;
int prevAutoMode =    0;

int sample = 0;
int sampleSize = 0;

int maxPercentage =   100;
int turnPercentage =  70;

void setup() {
  if(leftSide) {
    pinMode(enable12Pin, OUTPUT);
    pinMode(driver1Pin, OUTPUT);
    pinMode(driver2Pin, OUTPUT);
  }
  
  if(rightSide) {
    pinMode(enable34Pin, OUTPUT);
    pinMode(driver3Pin, OUTPUT);
    pinMode(driver4Pin, OUTPUT);
  }
  
  if(useArms) {
    armServo.attach(armPin);
    armServo.write(0);
  }
  
  pinMode(proximityPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ledPin, HIGH);
  
  Serial.begin(9600);
  Serial2.begin(1200); // Hardware supports up to 2400, but 1200 gives longer range
}

void loop() {
  if(useArms) {
    armServo.write(armPosition);
  }
 
  if(autoMode) {
    runAuto();
  } else {
    runRemote();
  }

  if(prevAutoMode != autoMode) {
    fullStop();
    prevAutoMode = autoMode;
  }

  processButton();
}

void processButton() {
  int buttonState = digitalRead(buttonPin);
  if((prevButtonState != buttonState) && (buttonState == 1)) {
    if(autoMode == 1) {
      autoMode = 0;
    } else {
      autoMode = 1;
    }
  }
  prevButtonState = buttonState;
}


void runAuto() {
  int distance = analogRead(proximityPin);
  sample += distance;
  sampleSize += 1;
  if(sampleSize > 10) {
    if((sample / 10) > 300) {
      backward45();
    }
    sample = 0;
    sampleSize = 0;
  }
  forward();
}

void runRemote() {
  Serial.println(readUInt(false));
}

void setTrackSpeed(TrackVal curSpeed) {
  if(((curSpeed.left > 5) || (curSpeed.left < -5)) && ((curSpeed.right > 5) || (curSpeed.right < -5))) { // operational range is X>5 && X<-5
    _leftTrack(curSpeed.left);
    _rightTrack(curSpeed.right);
  } else {
    fullStop();
  }
}

void _leftTrack(int percentage) {
  if(percentage != 0) { // not equal to zero means move
    digitalWrite(enable34Pin, HIGH);
    analogWrite(driver3Pin, ((percentage < 0)? ((255 * percentage) / 100) * -1 : 0));
    analogWrite(driver4Pin, ((percentage > 0)? ((255 * percentage) / 100) : 0));
  } else { // equal to zero means full stop
    digitalWrite(enable34Pin, LOW);
    analogWrite(driver3Pin, 0);
    analogWrite(driver4Pin, 0);
  }
}

void _rightTrack(int percentage) {
  if(percentage != 0) { // not equal to zero means move
    digitalWrite(enable12Pin, HIGH);
    analogWrite(driver1Pin, ((percentage > 0)? ((255 * percentage) / 100) : 0));
    analogWrite(driver2Pin, ((percentage < 0)? ((255 * percentage) / 100) * -1 : 0));
  } else { // equal to zero means full stop
    digitalWrite(enable12Pin, LOW);
    analogWrite(driver1Pin, 0);
    analogWrite(driver2Pin, 0);
  }
}

void forward() {
  _leftTrack(maxPercentage);
  _rightTrack(maxPercentage);
}

void fullStop() {
  _leftTrack(0);
  _rightTrack(0);
}

void backward45() {
  _leftTrack(maxPercentage * -1);
  _rightTrack(turnPercentage * -1);

  delay(1000); // delay to allow the bot to rotate a little
}

/*
void runMakeRemote() {
  int debug = 0;
  if (irrecv.decode(&irResults)) {
    if(debug) {
      Serial.println(irResults.value);
    }
    if(irResults.value == 0x962814CA) { // SW1
      if(debug) {
        Serial.println("Forward");
      } else {
        _leftTrack(100);
        _rightTrack(100);
      }
    } else if(irResults.value == 0xB2CC429A) { // SW2
      if(debug) {
        Serial.println("Left");
      } else {
        _leftTrack(-100);
        _rightTrack(100);
      }
    } else if(irResults.value == 0x2CD1795E) { // SW1 + SW2
      if(debug) {
        Serial.println("Forward + Left");
      } else {
        _leftTrack(50);
        _rightTrack(100);
      }
    } else if(irResults.value == 0x197F1446) { // SW3 + SW2
      if(debug) {
        Serial.println("Backward + Left");
      } else {
        _leftTrack(-50);
        _rightTrack(-100);
      }
    } else if(irResults.value == 0x5990708A) { // SW3
      if(debug) {
        Serial.println("Backward");
      } else {
        _leftTrack(-100);
        _rightTrack(-100);
      }
    } else if(irResults.value == 0xB012615A) { // SW4
      if(debug) {
        Serial.println("Right");
      } else {
        _leftTrack(100);
        _rightTrack(-100);
      }
    } else if(irResults.value == 0x2A17981E) { // SW1 + SW4
      if(debug) {
        Serial.println("Forward + Right");
      } else {
        _leftTrack(100);
        _rightTrack(50);
      }
    } else if(irResults.value == 0xED7FF3DE) { // SW3 + SW4
      if(debug) {
        Serial.println("Backward + Right");
      } else {
        _leftTrack(-100);
        _rightTrack(-50);
      }
    } else if(irResults.value == 0x8D2A4BAF) { // SW5
      // Serial.println("Arm Up");
    } else if(irResults.value == 0x1C22DE05) { // SW6
      // Serial.println("Arm Down");
    } else if(irResults.value == 0x7A6E10BA) { // SW7
      if(debug) {
        Serial.println("Full Stop");
      } else {
        fullStop();
      }
    } else if(irResults.value == 0x97123E8A) { // SW8
      if(debug) {
        Serial.println("Full Stop");
      } else {
        fullStop();
      }
    }
    irrecv.resume(); // Receive the next value
  } else if(false) {
    if(debug) {
      Serial.println("Full Stop");
    } else {
      fullStop();
    }
  }
}

void xbeeRemote() {  
  if(Serial2.available() > 11) {
    char xChar[4]; // Allocate some space for the left right value
    int xCharIndex = 0;
    int xVal = 0;
    char yChar[4]; // Allocate some space for the up down value
    int yCharIndex = 0;
    int yVal = 0;
    
    char tmpChar = 0;
    tmpChar = Serial2.read();
    while((tmpChar != '^')) { // find the beginning of the transmission
      tmpChar = Serial2.read();
    }
    
    tmpChar = Serial2.read(); // skip the ^ character
    
    while((tmpChar != '|')) { // parse out the left right value
      xChar[xCharIndex++] = tmpChar;
      tmpChar = Serial2.read();
    }
    xChar[xCharIndex++] = '\0';
    xCharIndex = 0;
    
    tmpChar = Serial2.read(); // skip the | character
    
    while((tmpChar != '$')) { // parse out the up down value
      yChar[yCharIndex++] = tmpChar;
      tmpChar = Serial2.read();
    }
    yChar[yCharIndex++] = '\0';
    yCharIndex = 0;
    
    sscanf(xChar, "%d", &xVal);
    sscanf(yChar, "%d", &yVal);
    
    TrackVal curSpeed = calcTrackSpeed(xVal, yVal);
    setTrackSpeed(curSpeed);
  }
}

void halfForward() {
  _leftTrack(50);
  _rightTrack(50);
}

void backward() {
  _leftTrack(maxPercentage * -1);
  _rightTrack(maxPercentage * -1);
}

void left() {
  _leftTrack(turnPercentage);
  _rightTrack(maxPercentage);
}

void fullLeft() {
  _leftTrack(0);
  _rightTrack(maxPercentage);
}

void right() {
  _leftTrack(maxPercentage);
  _rightTrack(turnPercentage);
}

void fullRight() {
  _leftTrack(maxPercentage);
  _rightTrack(0);
}

void raiseArm() {
  armPosition += 30;
  if(armPosition > 180) {
    armPosition = 180;
  }
}

void lowerArm() {
  armPosition -= 30;
  if(armPosition < 0) {
    armPosition = 0;
  }
}
*/ 

// Maurice Ribble 
// 8-30-2009
// http://www.glacialwanderer.com/hobbyrobotics
// Used Arduino 0017
// This does does some error checking to try to make sure the receiver on this one way RF 
//  serial link doesn't repond to garbage

#define NETWORK_SIG_SIZE 6

#define VAL_SIZE         2
#define CHECKSUM_SIZE    1
#define PACKET_SIZE      (NETWORK_SIG_SIZE + VAL_SIZE + CHECKSUM_SIZE)

// The network address byte and can be change if you want to run different devices in proximity to each other without interfearance
#define NET_ADDR 5

#define byte uint8_t
const byte g_network_sig[NETWORK_SIG_SIZE] = {0x8F, 0xAA, NET_ADDR};  // Few bytes used to initiate a transfer

// Sends an unsigned int over the RF network
void writeUInt(unsigned int val)
{
  byte checksum = (val/256) ^ (val&0xFF);
  Serial2.write(0xF0);  // This gets reciever in sync with transmitter
  Serial2.write(g_network_sig, NETWORK_SIG_SIZE);
  Serial2.write((byte*)&val, VAL_SIZE);
  Serial2.write(checksum); //CHECKSUM_SIZE
}

// Receives an unsigned int over the RF network
unsigned int readUInt(bool wait)
{
  int pos = 0;          // Position in the network signature
  unsigned int val;     // Value of the unsigned int
  byte c = 0;           // Current byte
  
  if((Serial2.available() < PACKET_SIZE) && (wait == false))
  {
    return 0xFFFF;
  }
  
  while(pos < NETWORK_SIG_SIZE)
  { 
    while(Serial2.available() == 0); // Wait until something is avalible
    c = Serial2.read();

    if (c == g_network_sig[pos])
    {
      if (pos == NETWORK_SIG_SIZE-1)
      {
        byte checksum;

        while(Serial2.available() < VAL_SIZE + CHECKSUM_SIZE); // Wait until something is avalible
        val      =  Serial2.read();
        val      += ((unsigned int)Serial2.read())*256;
        checksum =  Serial2.read();
        
        if (checksum != ((val/256) ^ (val&0xFF)))
        {
          // Checksum failed
          pos = -1;
        }
      }
      ++pos;
    }
    else if (c == g_network_sig[0])
    {
      pos = 1;
    }
    else
    {
      pos = 0;
      if (!wait)
      {
        return 0xFFFF;
      }
    }
  }
  return val;
}

/* vim: set tabstop=2 shiftwidth=2 expandtab: */
