#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// Switches (ON/OFF Sensors + Opto resistor for power of light reduce)
int SW1 = 14;           // PC0
int SW2 = 15;           // PC2
int OptoResistor = A3;  // PC4
// LED Strips
int LedStrip = 10;    // PB2
int LedStrip2 = 11;   // PB3
//int testLed1 = 13;    // PB1


// common variables
int fadeValue1 = 127;
int fadeValue2 = 127;
int maxFade = 30;

//bool LedStatus = false; // ON / OFF
//bool LedStatus2 = false; // ON / OFF

void setup() {
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
//  pinMode(testLed1, OUTPUT);
  pinMode(LedStrip, OUTPUT);
  pinMode(LedStrip2, OUTPUT);
  

  // выключаем тестовый светодиод
  //digitalWrite(testLed1, LOW);
  digitalWrite(LedStrip, LOW);
  digitalWrite(LedStrip2, LOW);

//digitalWrite(LedStrip, HIGH);
//digitalWrite(LedStrip2, HIGH);
  //delay(10000);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // backlight is ON
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Sw1:    Sw2:    ");
  lcd.setCursor(0,1);
  lcd.print("OR:    Fade:    ");
}

void loop() {
  // put your main code here, to run repeatedly:
  int ReadSW1 = 0;
  ReadSW1 = digitalRead(SW1);
  delay(10);

  int ReadSW2 = 0;
  ReadSW2 = digitalRead(SW2);
  delay(10);

  int sensorValueRAW = analogRead(OptoResistor);
  int sensorValue = constrain(sensorValueRAW, 150, 350);
  // Convert the analog reading
  maxFade = map(sensorValue, 150, 350, 30, 255);


  lcd.setCursor(5,0);
  lcd.print(ReadSW1);
  lcd.setCursor(13,0);
  lcd.print(ReadSW2);
  
  lcd.setCursor(3,1);
  lcd.print(sensorValueRAW);
  lcd.setCursor(12,1);
  lcd.print(maxFade);
  
  Serial.print("Switch-1: ");
  Serial.print(ReadSW1);
  Serial.print(", Switch-2: ");
  Serial.print(ReadSW2);
  Serial.print(", OptoResistor: ");
  Serial.print(sensorValue);
  Serial.print(", maxFade: ");
  Serial.print(maxFade);
  delay(1);


/*
  digitalWrite(testLed1, LOW);
      delay(1000);

  digitalWrite(testLed1, HIGH);
    delay(1000);
*/

  if ( ReadSW1 == 0 and fadeValue1 < maxFade )
  {
    fadeValue1++;
    analogWrite(LedStrip, fadeValue1);
  }
  else if ( ReadSW1 == 1 and fadeValue1 > 0 )
  {
    fadeValue1--;
    analogWrite(LedStrip, fadeValue1);
  }

  if ( ReadSW2 == 0 and fadeValue2 < maxFade )
  {
    fadeValue2++;
    analogWrite(LedStrip2, fadeValue2);
  }
  else if ( ReadSW2 == 1 and fadeValue2 > 0 )
  {
    fadeValue2--;
    analogWrite(LedStrip2, fadeValue2);
  }

/*

  if (ReadSW1 == 0)
  {
    if (LedStatus != true) 
    {
      digitalWrite(testLed1, HIGH);

      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
      }
      // после включения 10 секунд ждем безусловно. потом только начинаем пасти происходящее
      delay(10);

    }
    LedStatus = true;

  }
  if (ReadSW1 == 1) 
  {
    // fade out from max to min in increments of 5 points:
    if (LedStatus != false) 
    {
      digitalWrite(testLed1, LOW);
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        //Serial.println(fadeValue);
        delay(10);
      }
    }
    LedStatus = false;

  }
  if (ReadSW2 == 0)
  {
    if (LedStatus2 != true) 
    {
      digitalWrite(testLed1, HIGH);

      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip2, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
      }
      // после включения 10 секунд ждем безусловно. потом только начинаем пасти происходящее
      delay(10);

    }
    LedStatus2 = true;

  }
  if (ReadSW2 == 1) 
  {
    // fade out from max to min in increments of 5 points:
    if (LedStatus2 != false) 
    {
      digitalWrite(testLed1, LOW);
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip2, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        //Serial.println(fadeValue);
        delay(10);
      }
    }
    LedStatus2 = false;

  }
//*/
}
