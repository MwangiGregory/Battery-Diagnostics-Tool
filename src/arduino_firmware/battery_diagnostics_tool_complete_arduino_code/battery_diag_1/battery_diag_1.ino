#include <mcp_can.h>
#include <SPI.h>

// #define CRC_16_POLYNOMIALS 0xa001 //computing for cell balancing

// variables sent via CAN frame message 2
long unsigned int rxId = 0;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];     // array to store serial string
int cell_Volt_Delta = 0; // variable for delta between max cell voltage and min cell voltage
int maxCell;
int minCell;
int maxTemp;
int minTemp;
char batteryId[22]; // Battery serial number string

uint16_t volt, curr, remCap = 0;
uint16_t fullCap, rSOC, nCycles = 0;
uint16_t NT1, NT2, NT3 = 0;
uint16_t NT4, NT5, NT6 = 0;
uint16_t cell_1, cell_2, cell_3 = 0;
uint16_t cell_4, cell_5, cell_6 = 0;
uint16_t cell_7, cell_8, cell_9 = 0;
uint16_t cell_10, cell_11, cell_12 = 0;
uint16_t cell_13, cell_14, cell_15 = 0;
uint16_t cell_16, cell_17, cell_18 = 0;
uint16_t cell_19, cell_20 = 0;
// uint16_t cell_22, cell_23, cell_24 = 0;
// uint16_t cell_25, cell_26, cell_27 = 0;
// uint16_t cell_28, cell_29, cell_30 = 0;
uint16_t Bal_17_33, Bal_1_16, protection, software;

// variables for protections triggered

int k;
int protection_activated;

enum protection
{
    Cell_OVP = 1,
    Cell_UVP = 2,
    Pack_OVP = 4,
    Pack_UVP = 8,
    Chg_OTP = 16,
    Chg_UTP = 32,
    Dsg_OTP = 64,
    Dsg_UTP = 128,
    Chg_OCP = 256,
    Dsg_OCP = 512,
    Short_circuit = 1024,
    Afe_error = 2048 // front detection IC error
};

// checking for voltage and current readings fluctuations
int prev_pack_volt = 0;
int current_pack_volt;
int volt_fluctuation;

// variables to compute max, min and cell voltage delta in func

int Cell_Delta = 0;
// int arr[20] = {cell_1, cell_2, cell_3, cell_4, cell_5, cell_6, cell_7, cell_8, cell_9, cell_10, cell_11, cell_12, cell_13, cell_14, cell_15, cell_16, cell_17, cell_18, cell_19, cell_20}; // initialize array of cell voltages
// int max_cell = arr[0];
// int min_cell = arr[0];
int i;
int j;
int PackVolt; // to check whether the sum of cell voltage * 20 is equal to battery pack voltage
int l;        // variable for for loop to check whether the sum of cell voltage * 20 is equal to battery pack voltage
int m;        // variable for for loop to check whether cell voltages within (3V to 4.15V range)

unsigned long canID = 0x100;

const int CHUNK_SIZE = 350;

int TimeCounter = 0;

int startTime = millis();

// CAN Transceiver >> Arduino Nano
//  INT >> D7
//  SCK >> D13
//  SI >> D11
//  SO >> D12
//  CS >> D6
//  GND >> GND
//  VCC >> 5V

void PerformHandshake();
void GetBatteryBroadcastInfo();
void GetBatteryId();
void canMain();
void sendRemoteMessages(unsigned long canID);
void messageDetails(unsigned long ID, char msg[]);
// void MaxMinDeltaFunc();
void DiagnosticsReport();
void Protections();
int CANtimer = millis();

// setting can pins
#define CAN0_INT 2 // set INT to pin 7
#define CANCS 10
MCP_CAN CAN0(CANCS);
uint8_t count = 0;

void setup()
{
    Serial.begin(9600);
    // PerformHandshake();
    delay(1000);

    if (CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK)
        Serial.println(F("MCP2515 Initialized Successfully!"));
    else
        Serial.println(F("Error Initializing MCP2515..."));

    CAN0.setMode(MCP_NORMAL); // Set operation mode to normal so the MCP2515 sends acks to received data.

    pinMode(CAN0_INT, INPUT); // Configuring pin for /INT input
                              // digitalWrite(relay, HIGH);               //can Serial.println("MCP2515 Library Receive Example...");

    // GetBatteryId();
    // Print battery serial number

    Serial.println("BATTERY_SERIAL_NUMBER_START");
    Serial.println(batteryId);
    Serial.println("BATTERY_SERIAL_NUMBER_STOP");

    Serial.println("Delaying for 5 secs");
    delay(5000);
    // print all battery bradcast info: Highest cell voltage, lowest cell voltage, highest temperature reading and lowest temperature reading
    // GetBatteryBroadcastInfo();
}

void loop()
{
    int currentTime = millis();
    //
    if (currentTime - CANtimer > 2000)
    {
        // run can
        canMain();
        CANtimer = currentTime;
    }

    // MaxMinDeltaFunc(); //calling function to compute max, min and cell voltage delta
    // DiagnosticsReport();

    delay(1000);
}

void PerformHandshake()
{
#define PROGRAM_INIT_MESSAGE "BATTERY_DIAGNOSTICS_TOOL"
#define CONNECTED_MSG "CONNECTED"
#define HANDSHAKE_COMPLETE_MSG "HANDSHAKE_COMPLETE\n"

    String expected = CONNECTED_MSG;

    // if (Serial)
    // {
    Serial.println("BATTERY_DIAGNOSTICS_TOOL");

    while (!Serial.available())
    {
        /* Wait until you receive a message */
    }

    String response = Serial.readStringUntil('\0');

    if (response == expected)
        Serial.println("HANDSHAKE_COMPLETE");
    // }
}

// get battery broadcast data
void GetBatteryBroadcastInfo()
{
    // Check if receive buffer is ready to be read
    if (!digitalRead(CAN0_INT))
    {
        CAN0.readMsgBuf(&rxId, &len, rxBuf);
        // Check if the received id is the one representing battery broadcast data
        if ((rxId & 0x1FFFFFFF) == 0x10261051)
        {
            // maximum cell voltage
            maxCell = (rxBuf[0] << 8) | rxBuf[1];
            Serial.print(F("\nMaximum Cell Voltage"));
            Serial.println(maxCell);

            // minimum cell voltage
            minCell = (rxBuf[2] << 8) | rxBuf[3];
            Serial.print(F("\nMinimum Cell Voltage"));
            Serial.println(minCell);

            /// flag cell delta greater than 100mV
            cell_Volt_Delta = maxCell - minCell;
            Serial.print(F("\nCell Voltage Delta"));
            Serial.println(cell_Volt_Delta);
            if (cell_Volt_Delta > 100)
                Serial.print(F("The cell voltage delta is above the recommended threshold. Kindly take the battery to the battery lab for further troubleshooting"));

            // Highest temperature reading
            maxTemp = rxBuf[5];
            Serial.print(F("\nHighest Temperature Reading"));
            Serial.println(maxTemp);

            // Lowest temperature reading
            minTemp = rxBuf[6];
            Serial.print(F("\nMinimum Temperature Reading"));
            Serial.println(minTemp);
        }
    }
}

/**
 * GetBatteryId - read the serial number off of a Roam superpack
 * battery, that uses a baud rate of 250KB/s for its CAN bus.
 *
 * return - void
 */
void GetBatteryId()
{
    int batteryIdIndex = 0;
    const uint8_t BATTERY_ID_LAST_INDEX = 21;

    // Loop until you get the battery serial number
    while (true)
    {
        // Set the null terminating character
        if (batteryIdIndex == BATTERY_ID_LAST_INDEX)
        {
            batteryId[batteryIdIndex] = '\0';
            break;
        }
        // Check if receive buffer is ready to be read
        else if (!digitalRead(CAN0_INT))
        {
            CAN0.readMsgBuf(&rxId, &len, rxBuf);
            // Check if the received id is the one representing battery Id data
            if ((rxId & 0x1FFFFFFF) == 0x10261052)
            {
                /**
                 * Battery Id is received in 3 sections. Each data payload is
                 * 8 bytes long, with the MSB indicating the section number, e.g.
                 * 00, 01, 02. The MSB of each section is skipped.
                 * The rest of the bytes represent the sections of the battery
                 * serial number data.
                 * Here we get the characters from rxBuf and store the contiguously
                 * in the battery Id buffer.
                 */
                for (byte i = 0; i < len; i++)
                {
                    if (i == 0)
                    {
                        continue;
                    }
                    batteryId[batteryIdIndex] = rxBuf[i];
                    batteryIdIndex++;
                }
            }
        }
    }
}

void canMain()
{
    Serial.print(F(" \n \n  Calling CAN Function  \n "));
    /*
     *
      to receive battery information start by ding a remote CAN message

   */

    sendRemoteMessages(canID);

    canID++;

    if (canID > 0x110)
    {
        canID = 0x100;
    }

    if (digitalRead(CAN0_INT))
    {
        // Serial.println("CAN INT PIN HIGH 1");
    }
    else if (!digitalRead(CAN0_INT))
    {
        Serial.println(F(" CAN LINE LOW  READY TO RECEIVE DATA "));
    }
    if (!digitalRead(CAN0_INT)) // If CAN0_INT pin is low, read receive buffer
    {
        Serial.println(F("      Receiving CAN messages      "));
        CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read data: len = data length, buf = data byte(s)
        // Serial.println(" printing msg string ");

        if ((rxId & 0x80000000) == 0x80000000)
        { // Determine if ID is standard (11 bits) or extended (29 bits)
            // Serial.println(F("Extended ID:   DLC "));
            sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
        }
        else
        {
            sprintf(msgString, "Standard ID: 0x%.3x    DLC: %1d  Data:", rxId, len);
            messageDetails(rxId, rxBuf);
            // Serial.println(F("Standard ID:   DLC "));
        }
        Serial.print(msgString);

        if ((rxId & 0x40000000) == 0x40000000)
        { // Determine if message is a remote request frame.
            sprintf(msgString, " REMOTE REQUEST FRAME");

            Serial.print(msgString);
        }
        else
        {
            for (byte i = 0; i < len; i++)
            {
                sprintf(msgString, " 0x%.2X", rxBuf[i]);
                Serial.print(msgString);
            }
        }
        Serial.println();
    }
}

void sendRemoteMessages(unsigned long canID)
{

    CAN0.sendMsgBuf(canID | 0x40000000, 0, 0, 0);

    Serial.print(F(" \n \n \t  Sending Remote messages to the battery Waiting Response "));
    Serial.println(canID);
    delay(100);
}

void messageDetails(unsigned long ID, char msg[])
{
    switch (ID)
    {
    case 0x100:
    {
        Serial.println(F("\n Battery voltage, current, and Remaining Capacity "));

        // Voltage value
        volt = (msg[0] << 8) | msg[1];
        // Current value
        curr = (msg[2] << 8) | msg[3];

        // Remaining capacity
        remCap = (msg[4] << 8) | msg[5];

        // Value that indicates the start of streaming data values
        Serial.println(F("GENERIC_BATTERY_DATA_START"));

        Serial.print(F("\nBattery Voltage="));
        Serial.println(volt);

        Serial.print(F("\n Battery Current="));
        Serial.println(curr);

        Serial.print(F("\n Remaining Capacity="));
        Serial.println(remCap);
    }
    break;

    case 0x101:
    {
        Serial.println(F("\nBattery Full capacity, number of cycles, and relative rate of discharge"));

        // Full capacity
        fullCap = (msg[0] << 8) | msg[1];

        // Number of cycles
        nCycles = (msg[2] << 8) | msg[3];

        // Relative rate of discharge
        rSOC = (msg[4] << 8) | msg[5];

        Serial.print(F("\nBattery Full Capacity="));
        Serial.println(fullCap);

        Serial.print(F("\n\n Number of Cycles="));
        Serial.println(nCycles);

        Serial.print(F("\n\n Relative Rate of Discharge (%)="));
        Serial.println(rSOC);
    }
    break;

    case 0x102:
    {
        Serial.println(F("\nBalance of cell 1 to 16, Balance of cell 17 to 33, protection sign"));

        // Balance of cells 1 to 16
        Bal_1_16 = ((msg[0] << 8) | msg[1]);

        // Balance of cells 17 to 33
        Bal_17_33 = ((msg[2] << 8) | msg[3]);

        // Protection sign
        protection = ((msg[4] << 8) | msg[5]);

        Serial.print(F("\nBalance of cell 1 to 16="));
        Serial.println(Bal_1_16);

        Serial.print(F("\n\nBalance of cell 17 to 33="));
        Serial.println(Bal_17_33);

        Serial.print(F("\n\n Protection sign="));
        Serial.println(protection);
    }
    break;

    case 0x103:
    {
        Serial.println(F("\nSoftware Version"));

        // Software Version
        software = ((msg[0] << 8) | msg[1]);

        Serial.print(F("\nSoftware Version="));
        Serial.println(software, HEX); // print the software version in hexadecimal
    }
    break;

    case 0x105:
    {
        Serial.println(F("\n\n Temperature Values, NTC1, NTC2 NTC3 "));

        // Temperature values
        uint16_t val1 = ((msg[0] << 8) | msg[1]);
        uint16_t val2 = ((msg[2] << 8) | msg[3]);
        uint16_t val3 = ((msg[4] << 8) | msg[5]);

        NT1 = val1 + 2731 * 10;
        NT2 = val2 + 2731 * 10;
        NT3 = val3 + 2731 * 10;

        Serial.print(F("\n\n NTC1 Temp="));
        Serial.println(NT1);
        Serial.print(F("\n\n NTC2 Temp="));
        Serial.println(NT2);

        Serial.print(F("\n\n NTC3 Temp="));
        Serial.println(NT3);
        Serial.println(F("GENERIC_BATTERY_DATA_END"));
    }
    break;

    case 0x107:
    {
        Serial.println(F("\n\n Cell 1 Voltage, Cell 2 Voltage, Cell 3 Voltage "));

        // Cell 1 voltage value in mV
        cell_1 = ((msg[0] << 8) | (msg[1]));

        // Cell 2 voltage value in mV
        cell_2 = (msg[2] << 8) | msg[3];

        // Cell 3 voltage value in mV
        cell_3 = (msg[4] << 8) | msg[5];

        // Value that indicates the start of streaming cell data values
        Serial.println(F("CELL_DATA_START"));

        Serial.print(F("\n\n Cell 1 mV="));
        Serial.println(cell_1);
        Serial.print(F("\n\n Cell 2 mV="));
        Serial.println(cell_2);

        Serial.print(F("\n\n  Cell 3 mV="));
        Serial.println(cell_3);
      // int cellVoltages[3];

      // for (int i = 0, k = 0; i < 6, k < 3; i+=2, k++) {
      //   int cellVoltage = (rxBuf[i] << 8) | rxBuf[i + 1];
      //   cellVoltages[k] = cellVoltage;
      // }

      // for (int j = 0; j < 3; j++) {
      //   Serial.print("Cellvoltage: ");
      //   Serial.println(cellVoltages[j]);
    }
    break;

    case 0x108:
    {
        Serial.println(F("\n\n Cell 4 Voltage, Cell 5 Voltage, Cell 6 Voltage "));

        // Cell 4 voltage value in mV
        cell_4 = (msg[1] << 8) | msg[0];

        // Cell 5 voltage value in mV
        cell_5 = (msg[3] << 8) | msg[2];

        // Cell 6 voltage value in mV
        cell_6 = (msg[5] << 8) | msg[4];

        Serial.print(F("\n\n Cell 4 mV="));
        Serial.println(cell_4);
        Serial.print(F("\n\n Cell 5 mV="));
        Serial.println(cell_5);

        Serial.print(F("\n\n  Cell 6 mV="));
        Serial.println(cell_6);
    }
    break;

    case 0x109:
    {
        Serial.println(F("\n\n Cell 7 Voltage, Cell 8 Voltage, Cell 9 Voltage "));

        // Cell 7 voltage value in mV
        cell_7 = (msg[0] << 8) | msg[1];

        // Cell 8 voltage value in mV
        cell_8 = (msg[2] << 8) | msg[3];

        // Cell 9 voltage value in mV
        cell_9 = (msg[4] << 8) | msg[5];

        Serial.print(F("\n\n Cell 7 mV="));
        Serial.println(cell_7);
        Serial.print(F("\n\n Cell 8 mV="));
        Serial.println(cell_8);

        Serial.print(F("\n\n  Cell 9 mV="));
        Serial.println(cell_9);
    }
    break;

    case 0x10A:
    {
        Serial.println(F("\n\n Cell 10 Voltage, Cell 11 Voltage, Cell 12 Voltage "));

        // Cell 10 voltage value in mV
        cell_10 = (msg[0] << 8) | msg[1];

        // Cell 11 voltage value in mV
        cell_11 = (msg[2] << 8) | msg[3];

        // Cell 12 voltage value in mV
        cell_12 = (msg[4] << 8) | msg[5];

        Serial.print(F("\n\n Cell 10 mV="));
        Serial.println(cell_10);
        Serial.print(F("\n\n Cell 11 mV="));
        Serial.println(cell_11);

        Serial.print(F("\n\n  Cell 12 mV="));
        Serial.println(cell_12);
    }
    break;

    case 0x10B:
    {
        Serial.println(F("\n\n Cell 13 Voltage, Cell 14 Voltage, Cell 15 Voltage "));

        // Cell 13 voltage value in mV
        cell_13 = (msg[0] << 8) | msg[1];

        // Cell 14 voltage value in mV
        cell_14 = (msg[2] << 8) | msg[3];

        // Cell 15 voltage value in mV
        cell_15 = (msg[4] << 8) | msg[5];

        Serial.print(F("\n\n Cell 13 mV="));
        Serial.println(cell_13);
        Serial.print(F("\n\n Cell 14 mV="));
        Serial.println(cell_14);

        Serial.print(F("\n\n  Cell 15 mV="));
        Serial.println(cell_15);
    }
    break;

    case 0x10C:
    {
        Serial.println(F("\n\n Cell 16 Voltage, Cell 17 Voltage, Cell 18 Voltage "));

        // Cell 16 voltage value in mv
        cell_16 = (msg[0] << 8) | msg[1];

        // Cell 17 voltage value in mv
        cell_17 = (msg[2] << 8) | msg[3];

        // Cell 18 voltage value in mv
        cell_18 = (msg[4] << 8) | msg[5];

        Serial.print(F("\n\n Cell 16 mV="));
        Serial.println(cell_16);
        Serial.print(F("\n\n Cell 17 mV="));
        Serial.println(cell_17);

        Serial.print(F("\n\n  Cell 18 mV="));
        Serial.println(cell_18);
    }
    break;

    case 0x10D:
    {
        Serial.println(F("\n\n Cell 19 Voltage, Cell 20 Voltage, Cell 21 Voltage "));

        // Cell 19 voltage value in mV
        cell_19 = (msg[0] << 8) | msg[1];

        // Cell 20 voltage value in mV
        cell_20 = (msg[2] << 8) | msg[3];

        Serial.print(F("\n\n Cell 19 mV="));
        Serial.println(cell_19);
        Serial.print(F("\n\n Cell 20 mV="));
        Serial.println(cell_20);

        // Value that indicates the end of streaming cell data values
        Serial.println(F("CELL_DATA_END"));

        // MaxMinDeltaFunc(); // calling function to compute max, min and cell voltage
        DiagnosticsReport();
        // Value that indicates the end of streaming data values
        break;
    }
    }
}

// void MaxMinDeltaFunc()
// {

//     // Array to find min and max cells

//     for (int i = 0; i < 29; i++)
//     {
//         if (arr[i] > max_cell)
//         {
//             max_cell = arr[i];
//         }
//     }
//     for (int j = 0; j < 29; j++)
//     {
//         if (arr[j] < min_cell)
//         {
//             min_cell = arr[j];
//         }
//     }

//     Cell_Delta = max_cell - min_cell; // computing cell delta

//     Serial.println(F("MIN_MAX_DELTA_START"));
//     Serial.print(F("MaxCell="));
//     Serial.println(max_cell);
//     Serial.print(F("Cell Number="));
//     Serial.println(i + 1);
//     Serial.print(F("MinCell="));
//     Serial.println(min_cell);
//     Serial.print(F("Cell Number="));
//     Serial.println(j + 1);
//     Serial.print(F("Cell Voltage Delta="));
//     Serial.println(Cell_Delta);
//     Serial.println(F("MIN_MAX_DELTA_END"));
// }

void Protections()
{
    Serial.println(F("PROTECTIONS_DATA_START"));
    for (int k; k < 12; k++)
    {
        int protection_activated = protection & int(pow(2, k));

        if (protection_activated == Cell_OVP)
        {
            // Serial.println("START");
            Serial.println(F("=Cell_OVP happens when battery is charged to 4.2v"));
        }
        else if (protection_activated == Cell_UVP)
        {
            Serial.println(F("=Cell_UVP happens when the cell voltage is discharged to 2.75v and below"));
        }
        else if (protection_activated == Pack_OVP)
        {
            Serial.println(F("=Pack_OVP happens when pack charged and reaches 84v"));
        }
        else if (protection_activated == Pack_UVP)
        {
            Serial.println(F("=Pack_UVP happens when pack discharges to below  55v"));
        }
        else if (protection_activated == Chg_OTP)
        {
            Serial.println(F("=Chg_OTP happens when charging and temp reaches to 56 degrees (crux v1. 1) and 50 degrees (crux v1. 0)"));
        }
        else if (protection_activated == Chg_UTP)
        {
            Serial.println(F("=Chg_UTP happens when temp is below 0 degrees also when it has a  negative value"));
        }
        else if (protection_activated == Dsg_OTP)
        {
            Serial.println(F("=Dsg_OTP happens when temp is 66 degrees (crux v1. 1) and 60 degrees (crux v1. 0)"));
        }
        else if (protection_activated == Dsg_UTP)
        {
            Serial.println(F("=Dsg_UTP happens when temp is below 0 degrees"));
        }
        else if (protection_activated == Chg_OCP)
        {
            Serial.println(F("=Chg_OCP happens when current above 55A charges the battery. If triggered in idle state kindly take the battery to the lab for troubleshooting"));
        }
        else if (protection_activated == Dsg_OCP)
        {
            Serial.println(F("=Dsg_OCP Regen burst discharge current require by load above 120A. Should not occur in idle state but if it trips, check controller settings on maximum discharge current. Should be 80A"));
        }
        else if (protection_activated == Short_circuit)
        {
            Serial.println(F("=short circuit above 330A for 70sec. Disconnect any load from the battery, give it 30 seconds and interface again with tool. If the error persists, kindly take the battery to the lab for troubleshooting "));
        }
        else if (protection_activated == Afe_error)
        {
            Serial.println(F("=Afe_error occurs when bms connectors come into contact with the casing. Kindly take the battery to the lab for troubleshooting"));
            // Serial.println("END");
        }
    }
    Serial.println(F("PROTECTIONS_DATA_END"));
}

void DiagnosticsReport()
{
    Serial.println(F("REPORT_DATA_START"));
    Serial.println(F("Battery software version"));

    for (l = 0; l < 19; l++)
    {
        // PackVolt = arr[l] + arr[l + 1];
    }
    PackVolt = PackVolt * 20;
    if (PackVolt != volt)
    {
        Serial.println(F("The cell voltages do not add up to the pack voltage"));
    }
    if (volt < 5500)
    {
        Serial.println(F("Pack voltage below threshold. Kindly take battery to the lab for boost charging"));
    }
    if (volt > 8400)
    {
        Serial.println(F("Pack voltage above threshold. Kindly take battery to the lab for troubleshooting"));
    }
    // checking for voltage readings fluctuations
    volt_fluctuation = current_pack_volt - prev_pack_volt;
    if (volt_fluctuation > 2000)
    {
        Serial.println(F("Voltage readings fluctuation detected. Kindly take the battery to teh lab for troubleshooting"));
    }

    if (curr > 0)
    {
        Serial.println(F("Current reading despite battery being in idle state. Kindly take the battery to the lab for troubleshooting"));
    }

    // checking that all cell voltage readings are within 3V-4.15V and within 50mV of each other
    // for (m = 0; m < 20; m++)
    // {
    //     if (arr[m] > 4150)
    //     {
    //         Serial.println((arr[m]));
    //         Serial.println(F("Cell voltage is above the 4150mV threshold"));
    //     }
    //     if (arr[m] < 3000)
    //     {
    //         Serial.println((arr[m]));
    //         Serial.println(F("Cell voltage is below the 3000mV threshold"));
    //     }
    //     // iterate and check if difference between each cell voltage is greater than 50mV
    // }

    void protections();

    // if (Cell_Delta > 50)
    // {
    //     Serial.println(F("The cell delta is above 50mV threshold. Kindly take the battery to the lab for troubleshooting"));
    // }
    Serial.println(F("REPORT_DATA_END"));
}