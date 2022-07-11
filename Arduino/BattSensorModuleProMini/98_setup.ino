void xysetup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  tempSetup();
  adcSetup();
  
  // Switch LED on (turned off before deep sleep - turn on outside of deep sleep
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Setup ESP WakeUp pin
  pinMode(EspWakeUpPin, OUTPUT);
  digitalWrite(EspWakeUpPin, LOW);

  Serial.println("Setup completed.");

  // Run first x readouts from ADC (first values are sometimes wrong)
  for(int i=0; i<3; i++){
    AdcReadout();
  }

  //Serial.println("Scan for Temp Sensors (One Wire)");
  //scanSensor2();
  Serial.println("------------------------------------------------------");
}
