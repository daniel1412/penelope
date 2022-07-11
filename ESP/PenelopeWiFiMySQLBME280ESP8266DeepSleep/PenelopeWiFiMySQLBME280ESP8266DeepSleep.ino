// ToDo
// Capture disconnect wlan and reconnect
// TimeOut on WLAN connection

// Debug - No W-LAN Connection and no SQL Write
#define debug 0

// INCLUDES
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//-------------------------------Prepare for program-----------------------------
//-------------------------------------------------------------------------------
//-------------------------------User Setup (change settings here)---------------
// User Defined variables and defines
IPAddress sqlServerAddr(192,168,1,1);
#define ssid "PenelopeTestWLAN"
#define pass "3c90ab65d9a8f5c77d004634fdf92f1aaad57368b6bc8820c8802b7f09d7ab59"
#define sqlUser "penelopeWrite"
#define sqlPass "6c3a9fdf8b1177c64a0073269fa9e4d69f1cd54cb3d13aee7bc398379be1b665"
#define uS_TO_S_FACTOR 1000000    // Conversion factor for micro seconds to seconds
#if (debug == 1)
  #define TIME_TO_SLEEP 5
#else
  #define TIME_TO_SLEEP 57         // Time ESP32 will go to sleep (in seconds) - 6 Seconds for runtime  
#endif
#define LED_BUILTIN 2

// SQL Statements
char INSERT_SQL_T[] = "INSERT INTO Penelope.BME280_T_2(EnvSensorNr, Temperature, Humidity, Pressure, Altitude) VALUES(%d, %.2f, %.2f, %.2f, %.2f)";

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;  // I2C

unsigned long delayTime;

//-------------------------------- non User Setting vars (no changes needed)-----------------------
// global Variables
//RTC_DATA_ATTR int bootCount = 0;
//int mainLoopCount = 0;
//int updateIntervalTmp = 0;

// None User Define variables and defines
byte mac[6];
char sqlQuery[512];
WiFiClient client;
//MySQL_Connection conn((Client *)&client);

//-------------------------Functions----------------------------
//--------------------------------------------------------------

//--------------------------WiFi---------------------------------
void setupWiFi(){
  if(debug == 1){
    Serial.println("Skip Setup WiFi because of DEBUG option enabled");
  }
  else{
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
//-------------------------BME280-----------------------------------------
//------------------------------------------------------------------------

void printValues(){
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println();

  

  // Write to mySQL database
  sprintf(sqlQuery, INSERT_SQL_T, 2, temperature, humidity, pressure, altitude);
  Serial.print("SQL request: ");
  Serial.println(sqlQuery);
  MySQLwrite();
}

//-------------------------SETUP and Main LOOP----------------------------
//------------------------------------------------------------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  Serial.println("Penelope Project: BME280 sensor WiFi MySQL Esp8266 program");

  // Setup WiFi
  setupWiFi();
  
  // Init Sensors
  Serial.println("Initialising connection");
  
  // Run Sensor readout programs
  bool status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while(1);
  }

  printValues();
  
  // Go to Deep Sleep
  Serial.println("Going to sleep now");
  Serial.flush();
  digitalWrite(LED_BUILTIN, LOW);
  ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
  // Loop will never be reached
}
