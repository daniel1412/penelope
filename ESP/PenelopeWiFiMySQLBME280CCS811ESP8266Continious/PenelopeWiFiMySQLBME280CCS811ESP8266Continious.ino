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
#include <Adafruit_CCS811.h>

//-------------------------------Defines do not change---------------------------
//-------------------------------------------------------------------------------
#define FILLARRAY(a,n) a[0]=n, memcpy( ((char*)a)+sizeof(a[0]), a, sizeof(a)-sizeof(a[0]) );
#define uS_TO_S_FACTOR 1000000    // Conversion factor for micro seconds to seconds
#define LED_BUILTIN 2
//-------------------------------Prepare for program-----------------------------
//-------------------------------------------------------------------------------
//-------------------------------User Setup (change settings here)---------------
// User Defined variables and defines
#define sensorNr 5
#define maxRunsTillSend 60
IPAddress sqlServerAddr(192,168,178,50);
#define ssid "NTB"
#define pass "2eznhk85pw_xyz__"
#define sqlUser "penelopeWrite"
#define sqlPass "6c3a9fdf8b1177c64a0073269fa9e4d69f1cd54cb3d13aee7bc398379be1b665"

#if (debug == 1)
  #define TIME_TO_SLEEP 5
#else
  #define TIME_TO_SLEEP 57         // Time ESP32 will go to sleep (in seconds) - 6 Seconds for runtime  
#endif

// SQL Statements
//char INSERT_SQL_T[] = "INSERT INTO Penelope.BME280_T(EnvSensorNr, Temperature, Humidity, Pressure, Altitude, CO2, TVOC) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %i, %i)";
char INSERT_SQL_T[] = "INSERT INTO Penelope.BME280_CCS811_IN_2(EnvSensorNr, Temperature, TemperatureMin, TemperatureMax, Humidity, HumidityMin, HumidityMax, Pressure, PressureMin, PressureMax, Altitude, AltitudeMin, AltitudeMax, CO2, CO2Min, CO2Max, TVOC, TVOCMin, TVOCMax) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %i, %i, %i, %i, %i, %i)";

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;  // I2C
Adafruit_CCS811 ccs;

unsigned long delayTime;

float temperature;
float humidity;
float pressure;
float altitude;

int co2;
int TVOC;

float temperatureArray[maxRunsTillSend];
float temperatureMin;
float temperatureMax;
float humidityArray[maxRunsTillSend];
float humidityMin;
float humidityMax;
float pressureArray[maxRunsTillSend];
float pressureMin;
float pressureMax;
float altitudeArray[maxRunsTillSend];
float altitudeMin;
float altitudeMax;
int co2Array[maxRunsTillSend];
int co2Min;
int co2Max;
int TVOCArray[maxRunsTillSend];
int TVOCMin;
int TVOCMax;

int timeSinceStart = millis();
int timeLastRun = 0;
int countRuns = 0;

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
  Serial.println("MySQL write start");
  Serial.print("SQL query string = ");
  Serial.println(sqlQuery);
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

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);


  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" C");
  
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
  //sprintf(sqlQuery, INSERT_SQL_T, 3, temperature, humidity, pressure, altitude);
}

void getBMEValues(){
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" C");
  
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  // Save values to Array
  temperatureArray[countRuns] = temperature;
  humidityArray[countRuns] = humidity;
  pressureArray[countRuns] = pressure;
  altitudeArray[countRuns] = altitude;

  // Update min and max Values
  if(countRuns == 0){
    temperatureMin = temperature;
    temperatureMax = temperature;
    humidityMin = humidity;
    humidityMax = humidity;
    pressureMin = pressure;
    pressureMax = pressure;
    altitudeMin = altitude;
    altitudeMax = altitude;
  }
  else {
    // Temperature
    if(temperature < temperatureMin){
      temperatureMin = temperature;
    }
    if(temperature > temperatureMax){
      temperatureMax = temperature;
    }
    // Humidity
    if(humidity < humidityMin){
      humidityMin = humidity;
    }
    if(humidity > humidityMax){
      humidityMax = humidity;
    }
    // Pressure
    if(pressure < pressureMin){
      pressureMin = pressure;
    }
    if(pressure > pressureMax){
      pressureMax = pressure;
    }
    // Altitude
    if(altitude < altitudeMin){
      altitudeMin = altitude;
    }
    if(altitude > altitudeMax){
      altitudeMax = altitude;
    }
  }
}

void calculateMeanValuesBME(){
  float tempSum = 0.0;
  float humSum = 0.0;
  float preSum = 0.0;
  float altSum = 0.0;

  for(int i = 0; i < maxRunsTillSend; i++) {
    tempSum += temperatureArray[i];
    humSum += humidityArray[i];
    preSum += pressureArray[i];
    altSum += altitudeArray[i];
  }

  temperature = tempSum/maxRunsTillSend;
  humidity = humSum/maxRunsTillSend;
  pressure = preSum/maxRunsTillSend;
  altitude = altSum/maxRunsTillSend;
}

//-------------------------CCS811-----------------------------------------
//------------------------------------------------------------------------

// eCO2 values from 400 - 29206
// TVOC values from 0 - 32768


void readCCS811(){
  // Write temp and humidity to sensor
  //ccs.setEnvironmentalData(uint8_t(humidity), double(temperature));
  Serial.print("Write to CCS811 Temp = ");
  Serial.print(double(temperature));
  Serial.print(";;;; Humidity = ");
  Serial.println(uint8_t(humidity));

  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      co2 = ccs.geteCO2();
      Serial.print(co2);
      Serial.print("ppm, TVOC: ");
      TVOC = ccs.getTVOC();
      Serial.print(TVOC);
      Serial.println(" ppb");
    }
    else{
      Serial.println("ERROR during CCS811 read");
      while(1);
    }
  }


  //sprintf(sqlQuery, sqlQuery, String(co2), TVOC);
  //strcat(sqlQuery, String(co2));
  //strcat(sqlQuery, TVOC);
}

void getCCS811Values(){
  // Write temp and humidity to sensor
  //ccs.setEnvironmentalData(uint8_t(humidity), double(temperature));
  Serial.print("Write to CCS811 Temp = ");
  Serial.print(double(temperature));
  Serial.print(";;;; Humidity = ");
  Serial.println(uint8_t(humidity));

  while(!ccs.available()){
    delay(10);
  }

  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      co2 = ccs.geteCO2();
      Serial.print(co2);
      Serial.print("ppm, TVOC: ");
      TVOC = ccs.getTVOC();
      Serial.print(TVOC);
      Serial.println(" ppb");
    }
    else{
      Serial.println("ERROR during CCS811 read");
      while(1);
    }
  }
  else {
    Serial.println("CCS no data available");
  }
  
  // Save values to Array
  co2Array[countRuns] = co2;
  TVOCArray[countRuns] = TVOC;

  // Update min and max Values
  if(countRuns == 0){
    co2Min = co2;
    co2Max = co2;
    TVOCMin = TVOC;
    TVOCMax = TVOC;
  }
  else {
    // co2 Values
    if(co2 < co2Min){
      co2Min = co2;
    }
    if(co2>co2Max){
      co2Max = co2;
    }
    // TVOC Values
    if(TVOC < TVOCMin){
      TVOCMin = TVOC;
    }
    if(TVOC > TVOCMax){
      TVOCMax = TVOC;
    }
  }
}

void calculateMeanValuesCCS811(){
  long co2Sum = 0.0;
  long TVOCSum = 0.0;

  for(int i = 0; i < maxRunsTillSend; i++) {
    co2Sum += co2Array[i];
    TVOCSum += TVOCArray[i];
  }

  co2 = co2Sum/maxRunsTillSend;
  TVOC = TVOCSum/maxRunsTillSend;
}

//-------------------------SETUP and Main LOOP----------------------------
//------------------------------------------------------------------------
void setup() {
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Penelope Project: BME280 and CCS811 sensor WiFi MySQL Esp8266 program");

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
  //Serial.println("Run CCS Readout");
  //if(!ccs.begin()){
  //  Serial.println("Failed to start CCS811 sensor! Please check wiring!");
  //  while(1);
  //}

  // Wait for the CCS811 Sensor to be ready
  //while(!ccs.available());

  Serial.println("Run Readout");
  printValues();
  //readCCS811();
  //getCCS811Values();

  // Write to DB
  Serial.println("Run Query");
  sprintf(sqlQuery, INSERT_SQL_T, 3, temperature, humidity, pressure, altitude, co2, TVOC);
  MySQLwrite();
  
  // Go to Deep Sleep
  Serial.println("Going to sleep now");
  Serial.flush();
  digitalWrite(LED_BUILTIN, HIGH);
  ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
  //timeSinceStart = millis();
  //timeLastRun = 0;
  Serial.println("---------------------------------------------");
  // Dont use first X readout - sometimes sensor is not ready
  while(millis() < 20000){
    Serial.println("Initial wait for 20s");
    delay(1000);
  }
  timeSinceStart = millis();
  long timeTmp = timeSinceStart-timeLastRun;
  timeLastRun = timeSinceStart;
  Serial.print("Run Nr. = ");
  Serial.println(countRuns);
  Serial.print("Time Loop = ");
  Serial.println(timeTmp);
  Serial.println("");
  
  // read bme280e data and write to arrays
  getBMEValues();
  
  // read ccs811 data and write to arrays
  getCCS811Values();

  if(countRuns >= maxRunsTillSend - 1){
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Send collected data to DB");
    // calculate Mean values
    calculateMeanValuesBME();
    calculateMeanValuesCCS811();
    
    // send Values to db
    // temperature, temperatureMin, temperatureMax, humidity, humidityMin, humidityMax, pressure, pressureMin, pressureMax, altitude, altitudeMin, altitudeMax, co2, co2Min, co2Max, TVOC, TVOCMin, TVOCMax
    sprintf(sqlQuery, INSERT_SQL_T, sensorNr, temperature, temperatureMin, temperatureMax, humidity, humidityMin, humidityMax, pressure, pressureMin, pressureMax, altitude, altitudeMin, altitudeMax, co2, co2Min, co2Max, TVOC, TVOCMin, TVOCMax);
    MySQLwrite();
    
    // Reset variables for next round
    countRuns = 0;
    // Set all values of Array to -1
    FILLARRAY(temperatureArray,-32768);
    FILLARRAY(humidityArray,-32768);
    FILLARRAY(pressureArray,-32768);
    FILLARRAY(altitudeArray,-32768);
    FILLARRAY(co2Array,-32768);
    FILLARRAY(TVOCArray,-32768);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    // Update count variable
    countRuns += 1;
  }
}
