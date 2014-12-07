/* SevSeg Counter Example
 
 Copyright 2014 Dean Reading
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 
 This example demonstrates a very simple use of the SevSeg library with a 4
 digit display. It displays a counter that counts up, showing deci-seconds.
 */

#include "SevSeg.h"

SevSeg sevseg; //Instantiate a seven segment controller object


int adjustment = A3;  // ADC3 (CPU Pin 26) ( 10k adj resistor (to adjust time delay)

int incomingInt = 0;


void setup() 
{
  
  Serial.begin(9600);
  Serial.setTimeout(10);
  
  pinMode(adjustment, INPUT);
    
//  byte numDigits = 4;   
  byte numDigits = 3;   
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
//  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(100); // from 0 to 100
}

void loop() 
{
//  static unsigned long timer = millis();
//  static int deciSeconds = 0;
  
  
   if (Serial.available() > 0) 
   {
      incomingInt = Serial.parseInt();
//      Serial.write(incomingInt);
//      Serial.print(incomingInt);
   }

      sevseg.setNumber(incomingInt, 2);

/*
    if (incomingInt > 9999)
    {
      sevseg.setNumber(123, 1);
    }
    else 
    {
      sevseg.setNumber(568, 1);
    }
*/
//    sevseg.setNumber(123, 1);

/*   
  if (millis() >= timer) {
    deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond
    timer += 100; 
    if (deciSeconds == 1000) { // Reset to 0 after counting for 1000 seconds.
      deciSeconds=0;
    }
    sevseg.setNumber(deciSeconds, 1);
  }
*/


  sevseg.refreshDisplay(); // Must run repeatedly
  
//  delay(100);
}

/// END ///
