// CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>
byte incomingByte = 0;
byte freq = 0;
byte rate = 0;
bool configured = false;
long unsigned int rxId;
int retries = 0;
int speed = 0;
int maxspeed = 15;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  Serial.begin(57600);
  pinMode(CAN0_INT, INPUT);
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  /*if(CAN0.begin(MCP_ANY, speed, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
                              // Configuring pin for /INT input*/
}

void loop()
{
  if ((Serial.available() > 0)&& !configured) {
                // read the incoming byte:
    incomingByte = Serial.read(); //Тут идут в одном байте два числа. По 4 бита каждому. 00001111 0000 - freq 1111 - rate
                // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
	bitWrite(freq, 3, bitread(incomingByte, 7));
	bitWrite(freq, 2, bitread(incomingByte, 6));
	bitWrite(freq, 1, bitread(incomingByte, 5));
	bitWrite(freq, 0, bitread(incomingByte, 4));
	bitWrite(rate, 3, bitread(incomingByte, 3));
	bitWrite(rate, 2, bitread(incomingByte, 2));
	bitWrite(rate, 1, bitread(incomingByte, 1));
	bitWrite(rate, 0, bitread(incomingByte, 0));
	if (CAN0.begin(MCP_ANY, rate, freq) == CAN_OK){
		Serial.println("MCP2515 Initialized Successfully!");
		configured = true;
	}
	else
		Serial.println("Error Initializing MCP2515...");
	CAN0.setMode(MCP_NORMAL);
  }
  if (configured) {
    if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
      CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
      
      if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
        sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
      else
        sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    
      Serial.print(msgString);
      retries = 106; // Received successfully - baudrate is working.
      if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
        sprintf(msgString, " REMOTE REQUEST FRAME");
        Serial.print(msgString);
      } else {
        for(byte i = 0; i<len; i++){
          sprintf(msgString, " 0x%.2X", rxBuf[i]);
          Serial.print(msgString);
        }
      }
          
      Serial.println();
    }
	else 
	{
	  retries++;
	}
  }
	

  
    
  
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
