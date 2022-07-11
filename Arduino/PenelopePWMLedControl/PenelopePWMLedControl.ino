/* PWM control LED 24V stripe with push- rotate button
LED-Pins - Uno/Mini/Nano = 3,5,6,9,10,11 @ 480 Hz - 5,6 @ 960 Hz
*/

#include <Arduino.h>
#include <RotaryEncoder.h>

// PIN Setup
int ledPinWW = 5;   // pin for WarmWhite pwm output
int ledPinCW = 6;   // PIN FOR ColdWhite pwm output
int rotatePin1 = 2; // interrupt PINS on Uno/Nano/Mini only 2 and 3
int rotatePin2 = 3;
int buttonPin = 4;
int debounceCnt = 50;
int debounceCounter = 0;

// Fade setup PWM Led control
int fadeTime = 25;
int minFadeValue = 1;
int maxFadeValue = 254;
int startUpPwmValue = 25;
int pwmValueWW = startUpPwmValue;
int pwmValueCW = startUpPwmValue;
int pos = startUpPwmValue;
int minPos = 0;
int maxPos = 255;
bool lightState = 1;
bool buttonFree = 1;

RotaryEncoder *encoder = nullptr;

void checkPosition()
{
  //Serial.print("Button state = ");
  //Serial.println(digitalRead(buttonPin));
  encoder->tick();
}



void setup() {
  // Setup Serial
  Serial.begin(115200);
  // no setup for pwm needed
  pinMode(ledPinWW, OUTPUT);
  pinMode(ledPinCW, OUTPUT);
  TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz (The DEFAULT)
  analogWrite(ledPinWW, pwmValueWW);
  analogWrite(ledPinCW,  pwmValueCW);
  // LED Pin setup
  pinMode(LED_BUILTIN, OUTPUT);
  // Rotary Encoder Setup
  pinMode(rotatePin1, INPUT_PULLUP);
  pinMode(rotatePin2, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  encoder = new RotaryEncoder(rotatePin1, rotatePin2, RotaryEncoder::LatchMode::FOUR3);
  encoder->setPosition(pwmValueWW);
  attachInterrupt(digitalPinToInterrupt(rotatePin1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotatePin2), checkPosition, CHANGE);
}

void loop() {
  encoder->tick();
  int newPos = encoder->getPosition();
    
  if(pos != newPos) {
    //Serial Output Pos
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.print((int)(encoder->getDirection()));
    Serial.print(" ::::::: pos1:");
    Serial.print(digitalRead(rotatePin1));
    Serial.print(" ::: pos2:");
    Serial.println(digitalRead(rotatePin2));

    // 1-5 in 1er schritten
    // 6-50 in 5er schritten
    // 51-255 in 10er schritten
    if(newPos > 51)
    {
      int diff = (newPos - pos) * 10;
      newPos = pos + diff;
    }
    else if(newPos > 6)
    {
      int diff = (newPos - pos) * 5;
      newPos = pos + diff;
    }

    // Check min max values
    if(newPos < minPos){
      newPos = minPos;
    }
    if(newPos > maxPos){
      newPos = maxPos;
    }

    // Check if ligt was off bevore roation, if so start with pwm value 1
    if(lightState == 0)
    {
      newPos = minPos;
    }
    
    // Set pwmValues
    if(newPos == 0)
    {
      pwmValueWW = 1;
      pwmValueCW = 0;
    }
    else
    {
      pwmValueWW = newPos;
      pwmValueCW = newPos;
    }
    
    analogWrite(ledPinWW, pwmValueWW);
    analogWrite(ledPinCW, pwmValueCW);
    Serial.print("Write pwm Value: ");
    Serial.println(pwmValueWW);
    
    pos = newPos;
    encoder->setPosition(newPos);
    lightState = 1;
  }

  if(!buttonFree && digitalRead(buttonPin))
  {
    debounceCounter = debounceCounter + 1;
    if(debounceCounter > debounceCnt)
    {
      buttonFree = 1;
      debounceCounter = 0;
    }
  }
  if(buttonFree && !digitalRead(buttonPin) && !lightState)
  {
    buttonFree = 0;
    analogWrite(ledPinWW, pwmValueWW);
    analogWrite(ledPinCW, pwmValueCW);
    lightState = 1;
    Serial.print("Switch Light ON to pwm Value: ");
    Serial.println(pwmValueWW);
    digitalWrite(LED_BUILTIN, 1);
  }
  else if(buttonFree && !digitalRead(buttonPin) && lightState)
  {
    buttonFree = 0;
    analogWrite(ledPinWW, 0);
    analogWrite(ledPinCW, 0);
    lightState = 0;
    Serial.println("Switch Light OFF");
    digitalWrite(LED_BUILTIN, 0);
  }

  
  /*
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  // fade in from min to max
  for (int fadeValue = minFadeValue+1; fadeValue <= maxFadeValue; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPinWW, fadeValue);
    analogWrite(ledPinCW, fadeValue);
    // wait for xx milliseconds to see the dimming effect
    delay(fadeTime);
    Serial.println(fadeValue);
  }

  // fade out from max to min
  for (int fadeValue = maxFadeValue-1 ; fadeValue >= minFadeValue; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPinWW, fadeValue);
    analogWrite(ledPinCW, fadeValue);
    // wait for xx milliseconds to see the dimming effect
    delay(fadeTime);
    Serial.println(fadeValue);
  }
  */
}
