int count = 1;

#define BAUD_RATE 9600

void PerformHandshake()
{
#define PROGRAM_INIT_MESSAGE "BATTERY_DIAGNOSTICS_TOOL"
#define CONNECTED_MSG "CONNECTED"
#define HANDSHAKE_COMPLETE_MSG "HANDSHAKE_COMPLETE"

    String expected = CONNECTED_MSG;

    Serial.println(PROGRAM_INIT_MESSAGE);

    while (!Serial.available())
    {
    }

    String response = Serial.readStringUntil('\0');

    if (response == expected)
        Serial.println(HANDSHAKE_COMPLETE_MSG);
}

void setup()
{
    Serial.begin(BAUD_RATE);
    PerformHandshake();
}

void loop()
{
    if (count == 1)
    {
        Serial.println(F("GENERIC_BATTERY_DATA_START"));
        Serial.print(F("Battery Current="));
        Serial.println("4150");
    }
    else if (count == 2)
    {
        Serial.print(F("Remaining Capacity="));
        Serial.println(4150);
    }
    else if (count == 3)
    {
        Serial.print(F("Battery Full Capacity="));
        Serial.println(4150);
    }
    else if (count == 4)
    {
        Serial.print(F("Number Of Cycles="));
        Serial.println(4150);
    }
    else if (count == 5)
    {
        Serial.print(F("Relative Rate of Discharge="));
        Serial.println(4150);
    }
    else if (count == 6)
    {
        Serial.print(F("Balance of Cell 1 to 16="));
        Serial.println(4150);
    }
    else if (count == 7)
    {
        Serial.print(F("Balance of cell 17 to 33="));
        Serial.println(4150);
    }
    else if (count == 8)
    {
        Serial.print(F("Protection Sign="));
        Serial.println(4150);
    }
    else if (count == 9)
    {
        Serial.print("Software Version=");
        Serial.println(4150);
    }
    else if (count == 10)
    {
        Serial.print("NTC1 Temperature=");
        Serial.println(4150);
        Serial.print("Software Version=");
        Serial.println(4150);
    }
    else if (count == 11)
    {
        Serial.print("NTC2 Temperature=");
        Serial.println(4150);
    }
    else if (count == 12)
    {
        Serial.print("NTC3 Temperature=");
        Serial.println(4150);
    }
    else if (count == 13)
    {
        Serial.print("Cycles=");
        Serial.println(4150);
    }
    else if (count == 14)
    {
        Serial.print("Rate of Discharge=");
        Serial.println(4150);
    }
    else if (count == 15)
    {
        Serial.println(F("GENERIC_BATTERY_DATA_END"));
        Serial.println(F("CELL_DATA_START"));
        Serial.print(F("Cell 1 voltage="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 16)
    {
        Serial.print(F("Cell 2 voltage="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 17)
    {
        Serial.print(F("Cell 3 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 18)
    {
        Serial.print(F("Cell 4 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 19)
    {
        Serial.print(F("Cell 5 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 20)
    {
        Serial.print(F("Cell 6 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 21)
    {
        Serial.print(F("Cell 7 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 22)
    {
        Serial.print(F("Cell 8 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 23)
    {
        Serial.print(F("Cell 9 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 24)
    {
        Serial.print(F("Cell 10 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 25)
    {
        Serial.print(F("Cell 11 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 26)
    {
        Serial.print(F("Cell 12 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 27)
    {
        Serial.print(F("Cell 13 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 28)
    {
        Serial.print(F("Cell 14 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 29)
    {
        Serial.print(F("Cell 15 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 30)
    {
        Serial.print(F("Cell 16 voltage="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 31)
    {
        Serial.print(F("Cell 17 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 32)
    {
        Serial.print(F("Cell 18 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 33)
    {
        Serial.print(F("Cell 19 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 34)
    {
        Serial.print(F("Cell 20 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 35)
    {
        Serial.print(F("Cell 21 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 36)
    {
        Serial.print(F("Cell 22 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 37)
    {
        Serial.print(F("Cell 23 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 38)
    {
        Serial.print(F("Cell 24 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 39)
    {
        Serial.print(F("Cell 25 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 40)
    {
        Serial.print(F("Cell 26 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 41)
    {
        Serial.print(F("Cell 27 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 42)
    {
        Serial.print(F("Cell 28 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 43)
    {
        Serial.print(F("Cell 29 voltagee="));
        Serial.println(random(29, 42) / 10.0);
    }
    else if (count == 44)
    {
        Serial.print(F("Cell 30 voltagee="));
        Serial.println(random(29, 42) / 10.0);
        Serial.println(F("CELL_DATA_END"));
    }
    else if (count == 45)
    {
        Serial.println(F("MIN_MAX_DELTA_START"));

        Serial.print(F("MaxCell="));
        Serial.println(5000);

        Serial.print(F("Cell Number Max="));
        Serial.println(2);

        Serial.print(F("MinCell="));
        Serial.println(1000);

        Serial.print(F("Cell Number Min="));
        Serial.println(5);

        Serial.print(F("Cell Voltage Delta="));
        Serial.println(4000);

        Serial.println(F("MIN_MAX_DELTA_END"));
    }
    else if (count == 46)
    {
        Serial.println(F("PROTECTIONS_DATA_START"));

        Serial.println(F("Cell_OVP happens when battery is charged to 4.2v"));
        // Serial.println(F("=Cell_UVP happens when the cell voltage is discharged to 2.75v and below"));
        // Serial.println(F("=Pack_OVP happens when pack charged and reaches 84v"));
        // Serial.println(F("=Pack_UVP happens when pack discharges to below  55v"));
        // Serial.println(F("=Chg_OTP happens when charging and temp reaches to 56 degrees (crux v1. 1) and 50 degrees (crux v1. 0)"));
        // Serial.println(F("=Chg_UTP happens when temp is below 0 degrees also when it has a  negative value"));
        // Serial.println(F("=Dsg_OTP happens when temp is 66 degrees (crux v1. 1) and 60 degrees (crux v1. 0)"));
        // Serial.println(F("=Dsg_UTP happens when temp is below 0 degrees"));
        // Serial.println(F("=Chg_OCP happens when current above 55A charges the battery. If triggered in idle state kindly take the battery to the lab for troubleshooting"));
        // Serial.println(F("=Dsg_OCP Regen burst discharge current require by load above 120A. Should not occur in idle state but if it trips, check controller settings on maximum discharge current. Should be 80A"));
        // Serial.println(F("=short circuit above 330A for 70sec. Disconnect any load from the battery, give it 30 seconds and interface again with tool. If the error persists, kindly take the battery to the lab for troubleshooting "));
        // Serial.println(F("=Afe_error occurs when bms connectors come into contact with the casing. Kindly take the battery to the lab for troubleshooting"));
        Serial.println(F("PROTECTIONS_DATA_END"));
    }
    else if (count == 47)
    {
        Serial.println(F("REPORT_DATA_START"));
        Serial.println(F("Pack voltage below threshold. Kindly take battery to the lab for boost charging"));
        Serial.println(F("REPORT_DATA_END"));
    }

    // Serial.print("Cell 3 voltage value in mv: ");
    // Serial.println(4175);
    Serial.println();

    delay(200);

    count++;
    if (count == 48)
        count = 1;
}