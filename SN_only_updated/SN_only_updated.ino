#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2 // Set INT to pin 2
MCP_CAN CAN0(10); // Set CS to pin 10

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];

char batteryId[22];

void setup()
{
  Serial.begin(9600);
  
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("Read Super pack battery id example");

  getBatteryId(&rxId, &len, rxBuf, batteryId);

  sprintf(msgString, "Battery ID: %s" , batteryId);
  Serial.println(msgString);
}

void getBatteryId(unsigned long *id, byte *len, byte *buf, char *batteryIdBuf) {
  
  #define READY_TO_READ LOW

  bool recording = true;
  int batteryIdIndex = 0;
  byte rtrMsg[] = {0, 0, 0, 0, 0, 0, 0, 0};

  CAN0.sendMsgBuf(0x100, 8, rtrMsg); // Signal to wakeup battery bms

  while (recording) {

    if(digitalRead(CAN0_INT) == READY_TO_READ)
    {
      CAN0.readMsgBuf(id, len, buf);

      if ((*id & 0x1FFFFFFF) == 0x10261052)
      {
        for (char i = 1; i < *len; i++, batteryIdIndex++) {
          batteryIdBuf[batteryIdIndex] = buf[i];

          if ((buf[0] == (byte)0x02 && buf[i] == (byte)0x00) ||
              (buf[0] == (byte)0x02 && i == 7)) {

            recording = false;
            Serial.println(batteryIdIndex);
            Serial.println(rxBuf[i]);
            
            break;
          }
        }
      }
    }
  }
}

void loop()
{

}
