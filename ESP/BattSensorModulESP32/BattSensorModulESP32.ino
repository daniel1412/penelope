// First version of program to write temp values (from DS18B20 Sensor) to MariaDB via wlan connection

// INCLUDES
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ADS1256.h"

// User Defined variables and defines
#define ONE_WIRE_BUS 4  // Pin D2 on WeMos D1 Board (esp8266)
#define ssid "PenelopeTestWLAN"
#define pass "testtest"
#define sqlUser "penelopeWrite"
#define sqlPass "1234567890"
#define firstInterval 30000
#define updateInterval 60000
#define LED_BUILTIN 2

// global Variables
int mainLoopCount = 0;
int updateIntervalTmp = 0;
//char ssid[] = "PenelopeTestWLAN";
//char pass[] = "testtest";
//char sqlUser[] = "penelopeWrite";
//char sqlPass[] = "1234567890";
IPAddress sqlServerAddr(192,168,1,1);
//char INSERT_SQL[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Outside_T, Box_T, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T) VALUES('0', '26.1', '25.1', '1.1', '2.1', '3.1', '4.1', '5.1', '6.1', '7.1', '8.1')";
char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Outside_T, Box_T, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)";
char INSERT_SQL_V[] = "INSERT INTO Penelope.Batt_V(StackVoltage, VoltageCell1, VoltageCell2, VoltageCell3, VoltageCell4, VoltageCell5, VoltageCell6, VoltageCell7, VoltageCell8) VALUES(%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)";

// None User Define variables and defines
byte mac[6];
char sqlQuery[512];
WiFiClient client;
MySQL_Connection conn((Client *)&client);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ADS1256 ads;
unsigned long lasttime = millis();

void MySQLwrite()
{
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
  delete cur_mem;
  Serial.println("Query done");
}

void getAddressOfIndex(int index){
  Serial.print("Address of Sensor ");
  Serial.print(index);
  Serial.print(" is: ");
  uint8_t myPointer;
  //DeviceAddress myPointer;
  sensors.getAddress(&myPointer, index);
  Serial.println(myPointer);
}

void scanAddress(){
  for(uint8_t i = 0; i <= 255; i++) {
    Serial.print("Test Address: ");
    Serial.print(i);
    Serial.print(" valid: ");
    Serial.println(sensors.validAddress(&i));
    delay(5);
  }
}

void writeAdcToDB(){
  ads.readInputToAdcValuesArray();
  float value[7];
  Serial.println("ADC Values [V]:");
  for(int i = 0; i <= 7; i++){
    int valueRaw = ads.adcValues[i];
    float valueTmp = valueRaw*5.0/0x7fffff;
    value[i] = valueTmp;
    Serial.print(valueTmp);
    Serial.print("      ");
  }
  Serial.println();
  
  // Write to SQL db
  sprintf(sqlQuery, INSERT_SQL_V, value[0]+value[1]+value[2]+value[3]+value[4]+value[5]+value[6]+value[7], value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7]);
  Serial.print("SQL request: ");
  Serial.println(sqlQuery);
  MySQLwrite();
}


void writeTempToDB(){
  sensors.requestTemperatures();
  Serial.println("Temperatures [°C]:");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(1));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(2));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(3));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(4));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(5));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(6));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(7));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(8));
  Serial.print(" ; ");
  Serial.print(sensors.getTempCByIndex(9));
  Serial.println("");
  //Serial.print("Resolution is: ");
  //Serial.println(sensors.getResolution());
  //Serial.print("Number of Devices on Bus is: ");
  //Serial.println(sensors.getDeviceCount());
  //Serial.print("Number of DS18 Devices is: ");
  //Serial.println(sensors.getDS18Count());

  //scanAddress();
  // Write to SQL db
  sprintf(sqlQuery, INSERT_SQL_T, 1, sensors.getTempCByIndex(0), sensors.getTempCByIndex(1), sensors.getTempCByIndex(2), sensors.getTempCByIndex(3), sensors.getTempCByIndex(4), sensors.getTempCByIndex(5), sensors.getTempCByIndex(6), sensors.getTempCByIndex(7), sensors.getTempCByIndex(8), sensors.getTempCByIndex(9));
  Serial.print("SQL request: ");
  Serial.println(sqlQuery);
  MySQLwrite();    
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("DS18B20 Sensor readout to mySQL Database via WiFi connection with ESP8266 device");
  // Init Sensors
  Serial.println("Initialising connection");
  Serial.println("");
  sensors.begin();
  ads.init(5, 14, 25, 1700000);
  // Setup WiFi
  Serial.print("Connection to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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

void loop() {
  if(mainLoopCount == 0){
    updateIntervalTmp = firstInterval;
  }
  else{
    updateIntervalTmp = updateInterval;
  }
  if(millis() - lasttime > updateIntervalTmp){
    lasttime += updateIntervalTmp;
    // Set LED ON
    digitalWrite(LED_BUILTIN, HIGH);
    // Get ADC Sensor Data
    writeAdcToDB();
    // Get Temp Sensor data
    writeTempToDB();
    digitalWrite(LED_BUILTIN, LOW);
    mainLoopCount += 1;
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  // Setup loop time
  //delay(30000);
  //Serial.println("Pause between read out");
  //delay(30000);
  
}
