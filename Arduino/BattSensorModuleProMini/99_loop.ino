void xyloop() {
  // put your main code here, to run repeatedly:
  
  // read ADC data
  AdcReadout();
  //readByte(0x00);
  //getAdcValues();

  //Serial.print("Reg 0x00 content = ");
  //Serial.println(ads.readRegister(0x00));

  // read Temp data
  getTempValues();
  
  // activate ESP32 and send sql-query via uart - deactivate after received OK from ESP32
  digitalWrite(EspWakeUpPin, HIGH);
  delay(500);
  sendSqlStatement(INSERT_SQL_V_test, countVoltageValues, voltageValue, 6);
  sendSqlStatement(INSERT_SQL_T, countSensors, tempValues, 2);
  digitalWrite(EspWakeUpPin, LOW);
  
  // go to deep sleep
  Serial.println("--------------------------Go to sleep----------------------");
  startDeepSleep();
  Serial.println("--------------------------Wakeup from sleep ---------------");
  
  //delay(5000);
}
