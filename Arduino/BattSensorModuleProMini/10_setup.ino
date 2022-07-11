#define ipAddr0 192
#define ipAddr1 168
#define ipAddr2 178
#define ipAddr3 50

#define ONE_WIRE_BUS 3
#define tempPowerPin 6
#define EspWakeUpPin 2

#define TEMPERATURE_PRECISION 12

#define sqlUser "penelopeWrite"
#define sqlPass "6c3a9fdf8b1177c64a0073269fa9e4d69f1cd54cb3d13aee7bc398379be1b665"

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 16 // Must be dividable by 8 //294 - 2. 314 - 3. 354
#define runDeepSleepLoop TIME_TO_SLEEP/8

#ifndef LED_BUILTIN
  #define LED_BUILTIN 2
#endif

// ADC Values (count)
#define countVoltageValues 9

// SQL Vars
char sqlQuery[512];
//char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Outside_T, Box_T, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)";
//char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T, Box_T, Outside_T) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)";
//char INSERT_SQL_V[] = "INSERT INTO Penelope.Batt_V(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8) VALUES(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)";
//char INSERT_SQL_V_test[] = "INSERT INTO Penelope.Batt_V_test(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8, VoltageCell9) VALUES(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)";
char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T, Box_T, Outside_T) VALUES(1,";
char INSERT_SQL_V[] = "INSERT INTO Penelope.Batt_V(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8) VALUES(";
char INSERT_SQL_V_test[] = "INSERT INTO Penelope.Batt_V_test(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8, VoltageCell9) VALUES(";

// ADC Correction Values
//int CorrValuesADCRaw[] = {-4698, -9730, -14261, -18858, -20737, -25904, -29528, -33722};  // Diff = 
int CorrValuesADCRaw[] = {0, 0, 0, 0, 0, 0, 0, 0};  // linear equation -> y=13637*x+213.93
float ResistorFactor[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
//float ResistorFactor[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
