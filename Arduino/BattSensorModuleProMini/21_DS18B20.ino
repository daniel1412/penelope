//#include <OneWire.h>
//#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


// One-Wire Temp Sensor Address settings
#define countSensors 10
float CorrValues[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
DeviceAddress SensorAddress1 =  {0x28, 0x8A, 0xA9, 0x07, 0xD6, 0x01, 0x3C, 0x6C}; // Cell 1
//DeviceAddress SensorAddress2 =  {0x28, 0xDA, 0x92, 0x07, 0xD6, 0x01, 0x3C, 0xB6}; // Cell 2
DeviceAddress SensorAddress2 =  {0x28, 0xAC, 0xCF, 0x07, 0xD6, 0x01, 0x3C, 0xE5}; // Cell 2
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

float tempValues[countSensors];

void tempSetup(){
  //Switch Power on for Temp Sensors
  pinMode(tempPowerPin, OUTPUT);
  digitalWrite(tempPowerPin, HIGH);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void scanSensor2(){
  
  uint8_t address[8];
  uint8_t count = 0;
  
  if(oneWire.search(address)){
    do {
      count++;
      DeviceAddress TSensor;
      Serial.print("  {0x");
      for (uint8_t i = 0; i < 8; i++)
      {
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        TSensor[i] = address[i];
      }
      Serial.print("} Temp = ");
      Serial.print(sensors.getTempC(TSensor));
      Serial.println(",");
    } while (oneWire.search(address));
    Serial.print("Devices found: ");
    Serial.println(count);
  }
  else{
    Serial.println("No OneWire device available");
  }
  Serial.println("End Scan");
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
  //Serial.println("Temperatures [°C]:");
  for(int i = 0; i < countSensors; i++){
    temperatures[i] = sensors.getTempC(*SensorAddress[i])+CorrValues[i];
    //Serial.print(temperatures[i]);
    //Serial.print(" ; ");
  }
  //Serial.println("");
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
  while(tempValues[0] == -127.0 && tempValues[1] == -127.0 && tempValues[2] == -127.0 && tempValues[3] == -127.0 && tempValues[4] == -127.0 && tempValues[5] == -127.0 && tempValues[6] == -127.0 && tempValues[7] == -127.0){
    Serial.print("Invalid values found - repeat measurement");
    delay(500);
    countTmp = countTmp + 1;
    if(countTmp > 3){
      return;
    }
  }
  //scanAddress();
}
