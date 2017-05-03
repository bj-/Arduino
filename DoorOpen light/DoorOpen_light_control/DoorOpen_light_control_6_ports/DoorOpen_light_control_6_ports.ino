/* 
 *  Door Light Control 6 Ports
 *  
 *  version 1.0
 * 
 * Work on ATmega328 (16MHz internal)
 * 
 */
 #include <Wire.h> 
#include <LiquidCrystal_I2C.h> // https://github.com/marcoschwartz/LiquidCrystal_I2C

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Switches (ON/OFF Sensors + Opto resistor for power of light reduce)
int SW1 = 16;  // 8;           // PC0
int SW2 = 4;   //7;           // PC2
int SW3 = 15;  //4          // PC2
int SW4 = 7;   //14;           // PC2
int SW5 = 12;  //15;           // PC2
int SW6 = 8;   //16;           // PC2

// LED Strips
int LedStrip1 = 3; //11;    // PB2
int LedStrip2 = 9; //10;   // PB3
int LedStrip3 = 10; //9;   // PB3
int LedStrip4 = 11; //6;   // PB3
int LedStrip5 = 6; //5;   // PB3
int LedStrip6 = 5; //3;   // PB3


// common variables
int fadeValue1 = 50; // default current fade for strip
int fadeValue2 = 50;
int fadeValue3 = 50;
int fadeValue4 = 50;
int fadeValue5 = 50;
int fadeValue6 = 50;
int maxFade = 100;  //default max fade

int ReadSW1 = 0;
int ReadSW2 = 0;
int ReadSW3 = 0;
int ReadSW4 = 0;
int ReadSW5 = 0;
int ReadSW6 = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  pinMode(SW5, INPUT);
  pinMode(SW6, INPUT);
  pinMode(LedStrip1, OUTPUT);
  pinMode(LedStrip2, OUTPUT);
  pinMode(LedStrip3, OUTPUT);
  pinMode(LedStrip4, OUTPUT);
  pinMode(LedStrip5, OUTPUT);
  pinMode(LedStrip6, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // backlight is ON
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("S1-6: - - - - - ");
  lcd.setCursor(0,1);
  lcd.print("RAW:    Fade:   ");
  
}

int setPWMLevel(int sw, int LedStrip, int fadeValue, int maxFade, int channel)
{
  int ReadSW = digitalRead(sw); // read door switch
  delay(1);

  if ( ReadSW == 1 and fadeValue < maxFade )
  {
    fadeValue++;
    analogWrite(LedStrip, fadeValue);
  }
  else if  ( ReadSW == 1 and fadeValue > maxFade )
  {
    fadeValue--;
    analogWrite(LedStrip, fadeValue);
  }
  else if ( ReadSW == 0 and fadeValue > 0 )
  {
    fadeValue--;
    analogWrite(LedStrip, fadeValue);
  }

  switch (channel) 
  {
    case 1:
      lcd.setCursor(5,0);
      break;
    case 2:
      lcd.setCursor(7,0);
      break;
    case 3:
      lcd.setCursor(9,0);
      break;
    case 4:
      lcd.setCursor(11,0);
      break;
    case 5:
      lcd.setCursor(13,0);
      break;
    case 6:
      lcd.setCursor(15,0);
      break;
  }
  //lcd.print("Sw1-6: - - - - - ");
  lcd.print(ReadSW);
/*
  Serial.print("func setPWMLevel: SwitchPort: ");
  Serial.print(sw);
  Serial.print(", LedStripPort: ");
  Serial.print(LedStrip);
  Serial.print(" SwitchStatus: ");
  Serial.print(ReadSW);
  Serial.print(", fadeValue: ");
  Serial.print(fadeValue);
  Serial.print(", maxFade: ");
  Serial.println(maxFade);
*/
  return fadeValue;
}

void loop() {
  // put your main code here, to run repeatedly:

//  digitalWrite(LedStrip1, digitalRead(SW1));
//  digitalWrite(LedStrip2, digitalRead(SW2));
//  digitalWrite(LedStrip3, digitalRead(SW3));
//  digitalWrite(LedStrip4, digitalRead(SW4));
//  digitalWrite(LedStrip5, digitalRead(SW5));
//  digitalWrite(LedStrip6, digitalRead(SW6));

delay(1);


  fadeValue1 = setPWMLevel(SW1, LedStrip1, fadeValue1, maxFade, 1);
  fadeValue2 = setPWMLevel(SW2, LedStrip2, fadeValue2, maxFade, 2);
  fadeValue3 = setPWMLevel(SW3, LedStrip3, fadeValue3, maxFade, 3);
  fadeValue4 = setPWMLevel(SW4, LedStrip4, fadeValue4, maxFade, 4);
  fadeValue5 = setPWMLevel(SW5, LedStrip5, fadeValue5, maxFade, 5);
  fadeValue6 = setPWMLevel(SW6, LedStrip6, fadeValue6, maxFade, 6);

}
