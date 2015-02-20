int xPin = 17;
int yPin = 16;
int ledPin = 13;
int xOffset = 0;
int yOffset = 0;
int xVal = 0;
int yVal = 0;

void setup() {
  Serial2.begin(1200); // Hardware supports up to 2400, but 1200 gives longer range
  
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  readJoystick();
  xOffset = xVal * -1;
  yOffset = yVal * -1;
  
  digitalWrite(ledPin, HIGH);
}

void loop() {
  readJoystick();
  Serial2.println(((xVal + 100) * 1000) + (yVal + 100));
  delay(50);
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
