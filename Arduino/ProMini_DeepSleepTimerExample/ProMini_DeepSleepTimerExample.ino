/**
 * Author:Ab Kurk
 * version: 1.0
 * date: 24/01/2018
 * Description: 
 * This sketch is part of the beginners guide to putting your Arduino to sleep
 * tutorial. It is to demonstrate how to put your arduino into deep sleep and
 * how to wake it up.
 * Link To Tutorial http://www.thearduinomakerman.info/blog/2018/1/24/guide-to-arduino-sleep-mode
 */

#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#include <avr/wdt.h>  //this AVR library containt the methods that controls the watchdog
#define interruptPin 2 //Pin we are going to use to wake up the Arduino

volatile int sleepCnt = 0;

void setup() {
  Serial.begin(9600);//Start Serial Comunication
  pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is A sleep
  pinMode(interruptPin,INPUT_PULLUP);//Set pin d2 to input using the buildin pullup resistor
  digitalWrite(LED_BUILTIN,HIGH);//turning LED on
}

void loop() {
 digitalWrite(LED_BUILTIN,HIGH);//turning LED off
 Serial.println("In LOOP");
 delay(5000);//wait 5 seconds before going to sleep
 digitalWrite(LED_BUILTIN,LOW);//turning LED off
 Going_To_Sleep();
}

void Going_To_Sleep(){

    // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
    static byte prevADCSRA = ADCSRA;
    ADCSRA = 0;
    
    /* Set the type of sleep mode we want. Can be one of (in order of power saving):
     SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
     SLEEP_MODE_ADC
     SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
     SLEEP_MODE_EXT_STANDBY
     SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
     SLEEP_MODE_PWR_DOWN (Deep sleep)
    */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    sleep_enable();//Enabling sleep mode

    // Ensure we can wake up again by first disabling interrupts (temporarily) so
    // the wakeISR does not run before we are asleep and then prevent interrupts,
    // and then defining the ISR (Interrupt Service Routine) to run when poked awake by the tim
    noInterrupts();

    // clear various "reset" flags
    MCUSR = 0;  // allow changes, disable reset
    WDTCSR = bit (WDCE) | bit(WDE); // set interrupt mode and an interval
    WDTCSR = bit (WDIE) | bit(WDP3) | bit(WDP0); //| bit(WDP0);    // set WDIE, and 8 second delay
    wdt_reset();
        
    attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
    //digitalWrite(LED_BUILTIN,LOW);//turning LED off

    // Send a message just to show we are about to sleep
    Serial.println("Good night!");
    Serial.flush();

    // Allow interrupts now
    interrupts();
    
    delay(10); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    
    //Serial.println("just woke up!");//next line of code executed after the interrupt 
    //digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  }

void wakeUp(){
  Serial.println("Interrrupt Fired");//Print message to serial monitor
  sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;
  sleepCnt = 0;
}

// When WatchDog timer causes µC to wake it comes here
ISR (WDT_vect) {

  // Turn off watchdog, we don't want it to do anything (like resetting this sketch)
  wdt_disable();

  // Increment the WDT interrupt count
  sleepCnt++;

  Serial.println("ISR");
  Serial.print("SleepCounter = ");
  Serial.println(sleepCnt);

  // Now we continue running the main Loop() just after we went to sleep
  if(sleepCnt < 3){
    Going_To_Sleep();
  }
  else {
    sleepCnt = 0;
  }
}
