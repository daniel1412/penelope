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
  #define TIME_TO_SLEEP   294         // Time ESP32 will go to sleep (in seconds) - 6 Seconds for runtime  
#endif
#define LED_BUILTIN 2

// SQL Statements
char INSERT_SQL_T[] = "INSERT INTO Penelope.Batt_T(EnvSensorNr, Cell1_T, Cell2_T, Cell3_T, Cell4_T, Cell5_T, Cell6_T, Cell7_T, Cell8_T, Box_T, Outside_T) VALUES(%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)";

//-------------------------------- non User Setting vars (no changes needed)-----------------------
// global Variables
// only for ESP32
RTC_DATA_ATTR int bootCount = 0;

// None User Define variables and defines
byte mac[6];
char sqlQuery[512];
WiFiClient client;
MySQL_Connection conn((Client *)&client);

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
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
}

void startDeepSleep(){
  //ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR);  // ESP8266
  prepareDeepSleep();     // ESP32
  delay(50);              // ESP32
  esp_deep_sleep_start(); // ESP32
}

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
}

//-------------------------SETUP and Main LOOP----------------------------
//------------------------------------------------------------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  Serial.println("BASE Program WIFI MySQL for Penelope Project");
  
  // Check WakeUp condition and prepare sleep timer wakeup
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  //prepareDeepSleep();

  // Setup WiFi
  setupWiFi();
  
  // Init Sensors
  Serial.println("Initialising connection");
  Serial.println("");
  
  // Run Sensor readout programs

  
  // Go to Deep Sleep
  Serial.println("Going to sleep now");
  Serial.flush();
  digitalWrite(LED_BUILTIN, LOW);
  startDeepSleep();
}

void loop() {
  // Loop will never be reached
}
