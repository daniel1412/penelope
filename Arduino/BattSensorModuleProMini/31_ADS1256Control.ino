//#include "ADS1256.h"

//#include <ADS1256.h>
//#include <SPI.h>

float clockMHZ = 7.68; // crystal frequency used on ADS1256
float vRef = 2.5; // voltage reference

// Initialize ADS1256 object
ADS1256 adc(clockMHZ,vRef,true); // RESETPIN is permanently tied to 3.3v

float voltageValue[countVoltageValues];

void adcSetup2(){
  adc.begin(ADS1256_DRATE_50SPS,ADS1256_GAIN_1,false); 
  // Sending SDATAC to stop reading contionus data, so we can send other command
  adc.sendCommand(SDATAC);
  Serial.println("SDATAC command sent");
}

void adcSetup(){
  
  Serial.println("Starting ADC");

  // start the ADS1256 with data rate of 15 SPS
  // other data rates: 
  // ADS1256_DRATE_30000SPS
  // ADS1256_DRATE_15000SPS
  // ADS1256_DRATE_7500SPS
  // ADS1256_DRATE_3750SPS
  // ADS1256_DRATE_2000SPS
  // ADS1256_DRATE_1000SPS
  // ADS1256_DRATE_500SPS
  // ADS1256_DRATE_100SPS
  // ADS1256_DRATE_60SPS
  // ADS1256_DRATE_50SPS
  // ADS1256_DRATE_30SPS
  // ADS1256_DRATE_25SPS
  // ADS1256_DRATE_15SPS
  // ADS1256_DRATE_10SPS
  // ADS1256_DRATE_5SPS
  // ADS1256_DRATE_2_5SPS
  // 
  // NOTE : Data Rate vary depending on crystal frequency. Data rates listed below assumes the crystal frequency is 7.68Mhz
  //        for other frequency consult the datasheet.
  //Posible Gains 
  //ADS1256_GAIN_1 
  //ADS1256_GAIN_2 
  //ADS1256_GAIN_4 
  //ADS1256_GAIN_8 
  //ADS1256_GAIN_16 
  //ADS1256_GAIN_32 
  //ADS1256_GAIN_64 
  adc.begin(ADS1256_DRATE_50SPS,ADS1256_GAIN_1,true); 

  Serial.println("ADC Started");
  
  // Set MUX Register to AINCOM and AIN0 so it start doing the ADC conversion
  //adc.setChannel(7,8);  
}

void AdcReadoutTest(){
  Serial.println("Changing channel for differential mode.");

  for (int i = 0; i < 8; i++)
  {
    int answer = adc.setChannel(i,8);
    Serial.println(answer,BIN);
    delay(250);
    adc.waitDRDY();
    Serial.print("Current Channel: ");
    Serial.print(adc.readRegister(MUX),HEX); // Read the multiplex register to see the current active channel
    Serial.print(" = ");
    Serial.println(adc.readCurrentChannel());
  }
}

void readByte(unsigned char readRegisterNr){
  unsigned char answer;
  answer = adc.readRegister(readRegisterNr);
  Serial.print("Reg = ");
  Serial.println(answer,BIN);
}

void AdcReadout()
{ 
  int answer;
  // Efficient Input Cycling
  // to learn further, read on datasheet page 21, figure 19 : Cycling the ADS1256 Input Multiplexer

  answer = adc.setChannel(7,8);
  adc.waitDRDY();
  voltageValue[0] = adc.readCurrentChannel();
  
  answer = adc.setChannel(0,8);
  adc.waitDRDY();
  voltageValue[1] = adc.readCurrentChannel();
    
  answer = adc.setChannel(1,0);
  adc.waitDRDY();
  voltageValue[2] = adc.readCurrentChannel();
    
  answer = adc.setChannel(2,1);
  adc.waitDRDY();
  voltageValue[3] = adc.readCurrentChannel();
    
  answer = adc.setChannel(3,2);
  adc.waitDRDY();
  voltageValue[4] = adc.readCurrentChannel();
    
  answer = adc.setChannel(4,3);
  adc.waitDRDY();
  voltageValue[5] = adc.readCurrentChannel();
    
  answer = adc.setChannel(5,4);
  adc.waitDRDY();
  voltageValue[6] = adc.readCurrentChannel();
    
  answer = adc.setChannel(6,5);
  adc.waitDRDY();
  voltageValue[7] = adc.readCurrentChannel();
    
  answer = adc.setChannel(7,6);
  adc.waitDRDY();
  voltageValue[8] = adc.readCurrentChannel();
  
  //print the result.
//  for (uint8_t i = 0; i < countVoltageValues; i++)
//  {
//    Serial.print("V" + String(i) + ":");
//    Serial.print(voltageValue[i],6);
//    Serial.print(" ; ");
//  }
//  Serial.println();
}



//#include <ADS1256.h>
//#include <SPI.h>
//
//ADS1256 ads;
//float voltageValue[countVoltageValues];
//
//void adcSetup(){
//  ads.init(5, 14, 25, 100000); //pinCS, pinRDY, pinRESET, speedSPI 1700000
//  delay(500);  // give time for init ADS1256
//}
//
//void getAdcValues(){
//  ads.readInputToAdcValuesArray2();
//  //ads.readInputToAdcValuesArray();
//  //ads.readInputToAdcValuesArrayMulti();
//  Serial.println("ADC Values [V]:");
//  for(int i = 0; i < countVoltageValues; i++){
//    long valueRaw = ads.adcValues[i];
//    Serial.print("Raw ADC value (");
//    Serial.print(i);
//    Serial.print(") = ");
//    Serial.print(valueRaw,HEX);
//    Serial.print("     ");
//    //valueRaw += CorrValuesADCRaw[i];
//    //valueRaw += (0.0081*valueRaw+154,99);
//    float valueTmp = valueRaw*5.0/0x7fffff;
//    //valueTmp *= ResistorFactor[i];
//    voltageValue[i] = valueTmp;
//    Serial.print("Voltage = ");
//    Serial.print(valueTmp, 6);
//    Serial.println("      ");
//  }
//  Serial.println();
//}
//
//void AdcGetSqlStatement(){
//  // Write to SQL db
//  //sprintf(sqlQuery, INSERT_SQL_V, voltageValue[0]+voltageValue[1]+voltageValue[2]+voltageValue[3]+voltageValue[4]+voltageValue[5]+voltageValue[6]+voltageValue[7], voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7]);
//  //sprintf(sqlQuery, INSERT_SQL_V, voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7], voltageValue[8]);
//  sprintf(sqlQuery, INSERT_SQL_V_test, voltageValue[0], voltageValue[1], voltageValue[2], voltageValue[3], voltageValue[4], voltageValue[5], voltageValue[6], voltageValue[7], voltageValue[8]);
//  Serial.print("SQL request: ");
//  Serial.println(sqlQuery);
//  return sqlQuery;
//}
