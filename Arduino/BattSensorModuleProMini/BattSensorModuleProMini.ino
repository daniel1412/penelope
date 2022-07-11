/*
 * Control ADS1256 and DS18 Temp Sensor via Arduino ProMini 5V 328P Version
 * Pins to connect:
 * ADS1256 SPI:
 * SCK -> Pin13
 * MISO -> Pin12
 * MOSI -> Pin11
 * CS -> Pin10
 * RDY -> Pin9
 * RST -> Pin8
 * 
 * 
 * 
 * Temp Sensor - One Wire
 * Temp Sensor Power delivery -> Pin6
 * One-Wire -> Pin3
 * 
 * 
 * LED pin -> LED_BUILTIN -> Pin 13???
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ADS1256.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#define debug 0

//-------------------------------- SETUP FUNCTION (runs at first start) -----------------------

void setup() {
  xysetup();
}

//-------------------------------- MAIN Loop - runs endless ------------------------------------
void loop() {
  xyloop();
}
