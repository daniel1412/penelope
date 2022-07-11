// ToDo
// Capture disconnect wlan and reconnect
// TimeOut on WLAN connection

// Debug - No W-LAN Connection and no SQL Write
#define debug 0

// INCLUDES
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ADS1256.h"

//-------------------------------Prepare for program-----------------------------
//-------------------------------------------------------------------------------
//-------------------------------User Setup (change settings here)---------------
// User Defined variables and defines
#define TEMPERATURE_PRECISION 12
IPAddress sqlServerAddr(192,168,8,2);
//IPAddress sqlServerAddr(192,168,178,50);
#define ONE_WIRE_BUS 4  // Pin D2 on WeMos D1 Board (esp8266)
#define ssid "PenelopeSensorNET"
#define pass "(5SX(<2]hmXQ4yrJH]%Wyj]0[5]L-]T;hVp;i;ioJ2>P46h:Y4fY(0y:_9Yg"
//#define ssid "NTB"
//#define pass "2eznhk85pw_xyz__"
#define sqlUser "penelopeWrite"
#define sqlPass "6c3a9fdf8b1177c64a0073269fa9e4d69f1cd54cb3d13aee7bc398379be1b665"
//#define firstInterval 30000
//#define updateInterval 60000
#define uS_TO_S_FACTOR 1000000    // Conversion factor for micro seconds to seconds
#if (debug == 1)
  #define TIME_TO_SLEEP 2
#else
  #define TIME_TO_SLEEP   294    //294 - 2. 314 - 3. 354   // Time ESP32 will go to sleep (in seconds) - 6 Seconds for runtime  
#endif
#define LED_BUILTIN 2
#define countVoltageValues 9

// Sensor Address settings
#define countSensors 10
float CorrValues[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
DeviceAddress SensorAddress1 =  {0x28, 0x8A, 0xA9, 0x07, 0xD6, 0x01, 0x3C, 0x6C}; // Cell 1
DeviceAddress SensorAddress2 =  {0x28, 0xDA, 0x92, 0x07, 0xD6, 0x01, 0x3C, 0xB6}; // Cell 2
//DeviceAddress SensorAddress2 =  {0x28, 0xAC, 0xCF, 0x07, 0xD6, 0x01, 0x3C, 0xE5}; // Cell 2
DeviceAddress SensorAddress3 =  {0x28, 0x5A, 0x8A, 0x07, 0xD6, 0x01, 0x3C, 0x1E}; // Cell 3
DeviceAddress SensorAddress4 =  {0x28, 0xB4, 0x9D, 0x07, 0xD6, 0x01, 0x3C, 0x09}; // Cell 4
DeviceAddress SensorAddress5 =  {0x28, 0x4F, 0xA8, 0x07, 0xD6, 0x01, 0x3C, 0xD5}; // Cell 5
DeviceAddress SensorAddress6 =  {0x28, 0x75, 0xEA, 0x07, 0xD6, 0x01, 0x3C, 0x9D}; // Cell 6
DeviceAddress SensorAddress7 =  {0x28, 0x54, 0xB5, 0x07, 0xD6, 0x01, 0x3C, 0xE6}; // Cell 7
DeviceAddress SensorAddress8 =  {0x28, 0xB4, 0x9B, 0x07, 0xD6, 0x01, 0x3C, 0x95}; // Cell 8
DeviceAddress SensorAddress9 =  {0x28, 0x13, 0x86, 0x07, 0xD6, 0x01, 0x3C, 0xDC}; // Box inside
DeviceAddress SensorAddress10 = {0x28, 0xD9, 0xBB, 0x07, 0xD6, 0x01, 0x3C, 0xE4}; // Box outside
DeviceAddress *SensorAddress[] = {&SensorAddress1, 
                                  &SensorAddress2, 
                                  &SensorAddress3, 
                                  &SensorAddress4, 
                                  &SensorAddress5, 
                                  &SensorAddress6,
                                  &SensorAddress7,
                                  &SensorAddress8,
                                  &SensorAddress9,
                                  &SensorAddress10
                                  };

// SQL Statements
//char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Outside_T, Box_T, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)";
char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T, Box_T, Outside_T) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)";
char INSERT_SQL_V[] = "INSERT INTO Penelope.Batt_V(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8) VALUES(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)";
char INSERT_SQL_V_test[] = "INSERT INTO Penelope.Batt_V_test(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8, VoltageCell9) VALUES(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)";

// ADC Correction Values
//int CorrValuesADCRaw[] = {-4698, -9730, -14261, -18858, -20737, -25904, -29528, -33722};  // Diff = 
int CorrValuesADCRaw[] = {0, 0, 0, 0, 0, 0, 0, 0};  // linear equation -> y=13637*x+213.93
float ResistorFactor[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
//float ResistorFactor[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

//-------------------------------- non User Setting vars (no changes needed)-----------------------
// global Variables
RTC_DATA_ATTR int bootCount = 0;
//int mainLoopCount = 0;
//int updateIntervalTmp = 0;

// None User Define variables and defines
byte mac[6];
char sqlQuery[512];
WiFiClient client;
//MySQL_Connection conn((Client *)&client);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ADS1256 ads;
unsigned long lasttime = millis();
float voltageValue[countVoltageValues];
float tempValues[countSensors];

//-------------------------Functions----------------------------
//--------------------------------------------------------------
//-------------------------DeepSleep----------------------------
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void prepareDeepSleep(){
  print_wakeup_reason();
  //generate random number for Time to sleep -> less interference with BMS Measurment
  //int randomTime = random(-35, 35);
  int randomTime = 0;
  Serial.print("Random time part = ");
  Serial.println(randomTime);
  int timeToSleepTmp = TIME_TO_SLEEP;
  if(debug != 1){
    timeToSleepTmp = timeToSleepTmp + randomTime;
  }
  esp_sleep_enable_timer_wakeup(timeToSleepTmp * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(timeToSleepTmp) + " Seconds");
}

//--------------------------WiFi---------------------------------
void setupWiFi(){
  int timeoutCounts = 10; // x * delay in connection (500)
  if(debug == 1){
    Serial.println("Skip Setup WiFi because of DEBUG option enabled");
  }
  else{
    Serial.print("Connection to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    int count = 0;
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      count = count + 1;
      if(count > timeoutCounts){
        // Set deepSleep time to 20s and start deep sleep -> run will be tried again in 20s
        //esp_sleep_enable_timer_wakeup(20.0 * uS_TO_S_FACTOR);
        //Serial.println("Setup ESP to sleep for 20s -> because of error during wifi connection");
        //Serial.println("Going to sleep now");
        esp_deep_sleep_start();
      }
    }
    Serial.println("WiFi Connected");
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    Serial.print(mac[5],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.println(mac[0],HEX);
    Serial.println("");
    Serial.print("Assigned IP: ");
    Serial.print(WiFi.localIP());
    Serial.println("");
  }
}

//------------------------MySQL-----------------------
void MySQLwrite()
{
  if(debug == 1){
    Serial.println("SQLwrite skipped because of debug option enabled");
  }
  else{
    MySQL_Connection conn((Client *)&client);
    // Check if connection is established, if not make connection
    Serial.print("Connection Status = ");
    Serial.println(conn.connected());
    if(conn.connected() != 1)
    {
      Serial.print("Establish MySQL connection");
      while(conn.connect(sqlServerAddr, 3306, sqlUser, sqlPass) != true)
      {
        delay(500);
        Serial.print(".");
      }
      Serial.println("DONE");
    }
    // Write data to database
    Serial.println("Try to write sql query");
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(sqlQuery);
    cur_mem->close();
    delete cur_mem;
    Serial.println("Query done");
    conn.close();
  }
}

//------------------------DS18 Temp Sensor-------------------------
// function to print a device address
void printAddress(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void scanSensor(){
  sensors.requestTemperatures();
  delay(sensors.millisToWaitForConversion(TEMPERATURE_PRECISION));
  int deviceCount = sensors.getDeviceCount();
  int DS18Count = sensors.getDS18Count(); 
  Serial.print("Devices Found: ");
  Serial.println(deviceCount);
  //Serial.print(";;");
  //Serial.println(sensors.getDeviceCount(), DEC);
  Serial.print("DS18Sensor Found: ");
  Serial.println(DS18Count);
  // Request Temps
  //sensors.requestTemperatures();
  // Print Sensor addresses
  for(int i = 0; i < DS18Count; i++){
    Serial.print("Address of Sensor " + String(i) + " = ");
    DeviceAddress TSensor;
    sensors.getAddress(TSensor, i);
    printAddress(TSensor); 
    Serial.print(" ----- Temp = ");
    Serial.print(sensors.getTempC(TSensor));
    Serial.println("");
  }
}

void getTempByIndex(){
  sensors.requestTemperatures();
  delay(sensors.millisToWaitForConversion(TEMPERATURE_PRECISION));
  Serial.println("Temperatures [°C]:");
  for(int i = 0; i < countSensors; i++){
    Serial.print(sensors.getTempCByIndex(i));
    Serial.print(" ; "); 
  }
  Serial.println("");
}

void getTempBySensorAddress(){
  sensors.requestTemperatures();
  delay(sensors.millisToWaitForConversion(TEMPERATURE_PRECISION));
  Serial.println("Temperatures [°C]:");
  for(int i = 0; i < countSensors; i++){
    Serial.print(sensors.getTempC(*SensorAddress[i])+CorrValues[i]);
    Serial.print(" ; ");
  }
  Serial.println("");
}

void getAllSensorTemps(float temperatures[]){
  sensors.requestTemperatures();
  delay(sensors.millisToWaitForConversion(TEMPERATURE_PRECISION));
  Serial.println("Temperatures [°C]:");
  for(int i = 0; i < countSensors; i++){
    temperatures[i] = sensors.getTempC(*SensorAddress[i])+CorrValues[i];
    Serial.print(temperatures[i]);
    Serial.print(" ; ");
  }
  Serial.println("");
  //Serial.print("Resolution is: ");
  //Serial.println(sensors.getResolution());
  //Serial.print("Number of Devices on Bus is: ");
  //Serial.println(sensors.getDeviceCount());
  //Serial.print("Number of DS18 Devices is: ");
  //Serial.println(sensors.getDS18Count());
}


void getTempValues(){
  getAllSensorTemps(tempValues);
  // Check if all values are -127C - then run readout again - repeat max 3 times - skip readout and write to db if no useful data is available
  int countTmp = 0;
  while(tempValues[0] == -127.0 || tempValues[1] == -127.0 || tempValues[2] == -127.0 || tempValues[3] == -127.0 || tempValues[4] == -127.0 || tempValues[5] == -127.0 || tempValues[6] == -127.0 || tempValues[7] == -127.0 || tempValues[8] == -127.0 || tempValues[9] == -127.0){
    Serial.print("Invalid values found - repeat measurement");
    delay(1000);
    countTmp = countTmp + 1;
    if(countTmp > 3){
      return;
    }
  }
}

void writeTempValuesToDB(){
  // Write to SQL db
  sprintf(sqlQuery, INSERT_SQL_T, 1, tempValues[0], tempValues[1], tempValues[2], tempValues[3], tempValues[4], tempValues[5], tempValues[6], tempValues[7], tempValues[8], tempValues[9]);
  Serial.print("SQL request: ");
  Serial.println(sqlQuery);
  MySQLwrite();    
}

//-------------------------ADS1256 ADC converter--------------------------
void getAdcValues(){
  ads.readInputToAdcValuesArray3();
  //ads.readInputToAdcValuesArray();
  //ads.readInputToAdcValuesArrayMulti();
  Serial.println("ADC Values [V]:");
  for(int i = 0; i < countVoltageValues; i++){
    long valueRaw = ads.adcValues[i];
    Serial.print("Raw ADC value (");
    Serial.print(i);
    Serial.print(") = ");
    Serial.print(valueRaw,HEX);
    Serial.print("     ");
    //valueRaw += CorrValuesADCRaw[i];
    //valueRaw += (0.0081*valueRaw+154,99);
    float valueTmp = valueRaw*5.0/0x7fffff;
    //valueTmp *= ResistorFactor[i];
    voltageValue[i] = valueTmp;
    Serial.print("Voltage = ");
    Serial.print(valueTmp, 6);
    Serial.println("      ");
  }
  Serial.println();
}

void writeAdcValuesToDB(){
  // Write to SQL db
  //sprintf(sqlQuery, INSERT_SQL_V, voltageValue[0]+voltageValue[1]+voltageValue[2]+voltageValue[3]+voltageValue[4]+voltageValue[5]+voltageValue[6]+voltageValue[7], voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7]);
  sprintf(sqlQuery, INSERT_SQL_V, voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7], voltageValue[8]);
  //sprintf(sqlQuery, INSERT_SQL_V_test, voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7], voltageValue[8]);
  Serial.print("SQL request: ");
  Serial.println(sqlQuery);
  MySQLwrite();
}

//-------------------------SETUP and Main LOOP----------------------------
//------------------------------------------------------------------------
void setup() {
  WiFi.mode(WIFI_OFF);
  btStop();
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  Serial.println("DS18B20 Sensor readout and ADS1256 readout to mySQL Database via WiFi connection with ESP32 device");
  
  // Check WakeUp condition and prepare sleep timer wakeup
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  prepareDeepSleep();
  //digitalWrite(LED_BUILTIN, LOW);
  
  // Init Sensors
  Serial.println("Initialising connection");
  Serial.println("");
  sensors.begin();
  ads.init(5, 14, 25, 100000); //pinCS, pinRDY, pinRESET, speedSPI 1700000
  delay(500);  // give time for init ADS1256
  
  // Run Sensor readout programs
  getAdcValues();
  getTempValues();
  
  // Setup WiFi
  setupWiFi();

  // Write Values to DB
  writeAdcValuesToDB();
  writeTempValuesToDB();
  
  // Send ADC to Standby Mode
  //ads.goStandby();

  // Go to Deep Sleep
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Going to sleep now");
  WiFi.disconnect(true);
  Serial.flush();
  digitalWrite(LED_BUILTIN, LOW);
  //if(debug == 1){
  if(false){
    Serial.println("Skip DeepSleep because of debug option enabled");
  }
  else{
    esp_deep_sleep_start();
  }
}

void loop() {
  // Empty because of Deep Sleep in setup part-> loop never reached
}
