/* 
 *  Door Light Control
 *  
 *  version 1.0
 * 
 * Work on ATmega8A (8MHz internal)
 * 
 */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// Switches (ON/OFF Sensors + Opto resistor for power of light reduce)
int SW1 = 14;           // PC0
int SW2 = 16;           // PC2
int dSW = 13;            // PB1 (display and serial switch ON - debug info)
int OptoResistor = A3;  // PC4
// LED Strips
int LedStrip = 11;    // PB2
int LedStrip2 = 10;   // PB3


// common variables
int fadeValue1 = 50; // default current fade for strip
int fadeValue2 = 50;
int maxFade = 30;  //default max fade

void setup() {
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(dSW, INPUT);
  pinMode(LedStrip, OUTPUT);
  pinMode(LedStrip2, OUTPUT);
  

  // выключаем тестовый светодиод
  //digitalWrite(testLed1, LOW);
  digitalWrite(LedStrip, LOW);
  digitalWrite(LedStrip2, LOW);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // backlight is ON
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Sw1/2: /  sv:   ");
  lcd.setCursor(0,1);
  lcd.print("RAW:    Fade:   ");
}

void loop() {
  int ReadSW1 = 0;  // read door switch
  ReadSW1 = digitalRead(SW1);
  delay(1);

  int ReadSW2 = 0;
  ReadSW2 = digitalRead(SW2);
  delay(1);

  int ReadDSW = 0;    // read debug jumper switch
  ReadDSW = digitalRead(dSW);
  delay(1);

  int sensorValueRAW = analogRead(OptoResistor);
  int sensorValue = constrain(sensorValueRAW, 150, 350);  // require ajust after installation
  // Convert the analog reading
  maxFade = map(sensorValue, 150, 350, 30, 255);    // also ajust

  if ( ReadDSW == 0)  // debug module. for 16*2 LCD on I2C
  {
  //lcd.setCursor(0,0);
  //lcd.print("Sw1/2: /  sv:   ");
  //lcd.setCursor(0,1);
  //lcd.print("RAW:    Fade:   ");

  lcd.setCursor(6,0);
  lcd.print(ReadSW1);
  lcd.setCursor(8,0);
  lcd.print(ReadSW2);
  
  lcd.setCursor(4,1);
  lcd.print(sensorValueRAW);
  lcd.setCursor(13,0);
  lcd.print(sensorValue);
  lcd.setCursor(13,1);
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
  }
  else
  {
    delay(22);
  }

  if ( ReadSW1 == 0 and fadeValue1 < maxFade )
  {
    fadeValue1++;
    analogWrite(LedStrip, fadeValue1);
  }
  else if  ( ReadSW1 == 0 and fadeValue1 > maxFade )
  {
    fadeValue1--;
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
  else if  ( ReadSW2 == 0 and fadeValue2 > maxFade )
  {
    fadeValue2--;
    analogWrite(LedStrip2, fadeValue2);
  }
  else if ( ReadSW2 == 1 and fadeValue2 > 0 )
  {
    fadeValue2--;
    analogWrite(LedStrip2, fadeValue2);
  }
}
