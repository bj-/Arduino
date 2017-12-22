/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#include "pitches.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 7, 6, 5, 10);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


long timer = 390; // Timer in seconds (Start Value)
long DisplayContrast = 55;

int RelayPin = 2;
int LedStripPin = 3;
//int LeftButtonPin = 11;
//int CenterButtonPin = 12;
//int RightButtonPin = 9;
const int SoundPin = 4;

const int buttonsCnt = 3;
int buttonPin[buttonsCnt] = {9, 12, 11}; //Display buttons: Right - Center - Left
bool buttonClick[buttonsCnt] = {false, false, false};
bool ignoreButton[buttonsCnt] = {false, false, false};
int buttonTicCount[buttonsCnt] = {0, 0, 0};



#define TIME_BUTTON 3            // время устойчивого состояния кнопки (* +-2 мс) 


void setup()   {
  Serial.begin(9600);

  pinMode(SoundPin, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(LedStripPin, OUTPUT);

  // Set Button pins mode
  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
  //  Serial.println(buttonPin[i]);
    pinMode(buttonPin[i], INPUT);
  }
  
  digitalWrite(RelayPin, LOW);
  digitalWrite(LedStripPin, LOW);
  digitalWrite(SoundPin, LOW);

  // Read Stored values in EEPRom
  //Reading and sending first long.
  //int valuesInEEPROM = EEPROM.read(0);
  Serial.print("EEPRom Flag: ");
  Serial.print(EEPROM.read(0));
  
  if ( EEPROM.read(0) != 1 )
  {
    EEPROM.write(0, 1);
    EEPROMWritelong(1, timer);
    EEPROMWritelong(5, DisplayContrast);
  }
//    Serial.println(EEPROMReadlong(0));
  timer = EEPROMReadlong(1); // Read timer
  DisplayContrast = EEPROMReadlong(5); // Read Contrast


  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(DisplayContrast);
  display.clearDisplay();
  display.setRotation(0);

  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(20,10);
  display.println("Coundown");
  display.setCursor(30,35);
  display.println("Timer");
  display.display();


  //writeDefaultScreen(timer);
  //writeDefaultScreen();
/*  int timeToDownMinutes=timer/60;
  int timeToDownSeconds=timer%60;
    
  display.clearDisplay();
  writestatictext();
  
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(13,15);
  display.println(addleaderzero(timeToDownMinutes));
  display.setCursor(43,15);
  display.println(addleaderzero(timeToDownSeconds));
  display.display();
  */
  /*
  display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
*/

/*
  // rotation example
  display.clearDisplay();
  display.setRotation(1);  // rotate 90 degrees counter clockwise, can also use values of 2 and 3 to go further.
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Rotation");
  display.setTextSize(2);
  display.println("Example!");
  display.display();
  delay(2000);
*/
  // revert back to no rotation
//  display.setRotation(0);

/*  // invert the display
  display.invertDisplay(true);
  delay(1000); 
  display.invertDisplay(false);
  delay(1000); 

  testfillrect();
  display.display();
  delay(2000);
  display.clearDisplay();
*/  
}



unsigned long startTime = 0;  //When count down timer stared
String addZero = ""; // if minutes or secinds less than 10 - add 0 before value
bool timerWorking = false;
//int timeToDown = 2;
unsigned long timeToDown = 1;
int timeToDownMinutes = 0;
int timeToDownSeconds = 0;

bool noMusic = false;
bool Debug = false;
bool inMenu = false;

// Menu navigation varisbles
int menuCursorPosition = 0;
int menuCursorPositionMax = 30;
int menuLevel = 1;


//unsigned long buttonPressedTime = 0;


void ReadButtonState (int i)
{
  // Read buttons state
  // Left Button
  if ( digitalRead(buttonPin[i]) == true )
  {
    // если кнопка нажата, считаем тики
    buttonTicCount[i]++;   // +1 к счетчику состояния кнопки

    if ( buttonTicCount[i] >= TIME_BUTTON ) 
    {
      // состояние кнопки не мянялось в течение заданного времени - состояние кнопки стало устойчивым
      buttonClick[i] = true;
    }
  }
  else
  {
    buttonTicCount[i] = 0; // сброс счетчика подтверждений состояния кнопки
    buttonClick[i] = false;
    ignoreButton[i] = false; // сброс флага игнорирования нажатия кнопки. дабы небыло другой реакции на тоже самое нажатие.
  }

}

void loop() {

  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
    ReadButtonState (i);
  }


//  if ( inMenu )
//  {
    // Отдельное поведение для навигации по меню
//    Serial.println("inMenu");
//  }
//  else
//  {
/*    Serial.print("NormMode: ");
    Serial.print("buttonClick");
    Serial.print(buttonClick[2]);
    Serial.print("; timerWorking");
    Serial.print(timerWorking);
    Serial.print("; ignoreButton: ");
    Serial.println(ignoreButton[2]);
//*/
    // Start Timer and switch relay
    if ( buttonClick[2] == true and timerWorking == false and ignoreButton[2] == false and inMenu == false )
    {
      timerWorking = true;
      startTime = millis()/1000;
      digitalWrite(RelayPin, HIGH);
      ignoreButton[2] = true;
      noMusic = false; // reset to default
      Serial.println("Button3 = click, Relay = switch on");

    }
    else if ( buttonClick[2] == true and timerWorking == true and ignoreButton[2] == false and inMenu == false )
    {
      timerWorking = false;
      digitalWrite(RelayPin, LOW);
      ignoreButton[2] = true;
      noMusic = true; // do not play music if stopped manualy
      Serial.println("Button3 = click, Relay = switch OFF");
    }

    
    if ( timerWorking == true )
    {
      timeToDown=timer - ( millis()/1000 - startTime );
      timeToDownMinutes=timeToDown/60;
      timeToDownSeconds=timeToDown%60;

      if ( timeToDown < 1 )
      {
        timerWorking = false;
        digitalWrite(RelayPin, LOW);
        digitalWrite(LedStripPin, HIGH);
        if ( noMusic == false )
        {
          // Play only when ended by timer  
          playMusicStarWars();
          //noMusic = false;
        }
      }
    }
    else
    {
      timeToDownMinutes=timer/60;
      timeToDownSeconds=timer%60;
    }


        // On/Off Led Strip
    if ( buttonClick[0] == true and ignoreButton[0] == false and inMenu == false )
    {
      digitalWrite(LedStripPin, !digitalRead(LedStripPin));  //invert pin state
      ignoreButton[0] = true;
      Serial.println("Button1(Led) = click");

    }

//  }


  if ( buttonClick[1] == true and ignoreButton[1] == false and inMenu == false )
  {
      inMenu = true;
      ignoreButton[1] = true;
      Serial.println("Button2(Menu) = click");
    }


  if ( inMenu )
  {
    if ( menuLevel == 1 )
    {
      if ( buttonClick[0] == true and ignoreButton[0] == false and inMenu == true )
      {
        menuCursorPosition = menuCursorPosition + 10;
        if ( menuCursorPosition > menuCursorPositionMax ) menuCursorPosition = menuCursorPositionMax;
        ignoreButton[0] = true;
      }
      if ( buttonClick[2] == true and ignoreButton[2] == false and inMenu == true )
      {
        menuCursorPosition = menuCursorPosition - 10;
        if ( menuCursorPosition < 0 ) menuCursorPosition = 0;
        ignoreButton[2] = true;
      }

      if ( buttonClick[1] == true and ignoreButton[1] == false and inMenu == true )
      {
        if ( menuCursorPosition == 30 )
        {
          inMenu = false;
        }
        else if ( menuCursorPosition == 20 )
        {
          Debug = !Debug;
          inMenu = false;
        }
        else
        {
          menuLevel = 2;
        }
        ignoreButton[1] = true;
      }
      
    }
    else 
    {
      // Menu Level 2
      if ( menuCursorPosition == 0 )
      {
        // Timer Set
        if ( buttonClick[2] == true and inMenu == true )
        {
          timer--;
          if (timer < 1 ) timer = 1;
        }
        if ( buttonClick[0] == true and inMenu == true )
        {
          timer++;
        }
        timeToDownMinutes=timer/60;
        timeToDownSeconds=timer%60;

        if ( buttonClick[1] == true and ignoreButton[1] == false and inMenu == true )
        {
          // Save and Exit
          EEPROMWritelong(1, timer);
          //EEPROM.write(0, 1); // flag - stored in memory
          //Serial.print("timer current and read from mem: ");
          //Serial.print(timer);
          //Serial.print(" - ");
          //Serial.println(EEPROMReadlong(1));
          ignoreButton[1] = true;
          menuLevel = 1;
        }
      }
      else if ( menuCursorPosition == 10 )
      {
        // Contrast Set
        if ( buttonClick[2] == true and ignoreButton[2] == false and inMenu == true )
        {
          DisplayContrast--;
          if (DisplayContrast < 50 ) DisplayContrast = 50;
          ignoreButton[2] = true;
        }
        if ( buttonClick[0] == true and ignoreButton[0] == false and inMenu == true )
        {
          DisplayContrast++;
          if (DisplayContrast > 70 ) DisplayContrast = 70;
          ignoreButton[0] = true;
        }
        display.setContrast(DisplayContrast);


        if ( buttonClick[1] == true and ignoreButton[1] == false and inMenu == true )
        {
          // Save and Exit
          EEPROMWritelong(5, DisplayContrast);
          ignoreButton[1] = true;
          menuLevel = 1;
        }
      }
      else
      {
        // impossible way. Exit.
        inMenu = false;
      }
    }



  //Serial.print("bt:");
  //Serial.print(buttonClick[2]);
  //Serial.println(ignoreButton[2]);
    
    display.clearDisplay();
    display.setTextColor(BLACK);

    display.setTextSize(1);
    if ( menuLevel == 1 )
    {
      display.setCursor(0,0);
      display.println("   Timer");
      display.setCursor(0,10);
      display.println("   Contrast");
      display.setCursor(0,20);
      display.println("   Debug");
      display.setCursor(0,30);
      display.println("   Exit");
      // Set Cursor
      display.setCursor(0,menuCursorPosition);
      display.println(" >");

      display.setCursor(30,40);
      display.println("Enter");
    }
    else
    {
      // Menu Level 2
      display.setCursor(30,40);
      display.println("Ok");
      
      display.setTextSize(2);
      if ( menuCursorPosition == 0 )
      {
        display.setCursor(33,15);
        display.println(":");
        display.setCursor(13,15);
        display.println(addleaderzero(timeToDownMinutes));
        display.setCursor(43,15);
        display.println(addleaderzero(timeToDownSeconds));
      }
      else if ( menuCursorPosition == 10 )
      {
        display.setCursor(30,15);
        display.println(DisplayContrast);
      }
      display.setTextSize(1);
    }
      display.setCursor(0,40);
      display.println("<<");
      display.setCursor(65,40);
      display.println(">>");
    

    display.display();
  }
  else
  {
    // Write to Display
    display.clearDisplay();
    display.setTextColor(BLACK);
  
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Coundown Timer");
    display.setCursor(0,40);
    if ( timerWorking )
    {
      display.println("Stop");
    }
    else
    {
      display.println("Run");
    }
    display.setCursor(30,40);
    display.println("Menu");
    display.setCursor(65,40);
    display.println("Led");

    
    display.setTextSize(2);
    display.setCursor(33,15);
    display.println(":");
    display.setCursor(13,15);
    display.println(addleaderzero(timeToDownMinutes));
    display.setCursor(43,15);
    display.println(addleaderzero(timeToDownSeconds));
  
    // Add Debug Text on screen
    if ( Debug ) 
    {
      display.setTextSize(1);
      display.setCursor(0,31);
      //display.println("DBGline2222222"); //14 sybb in one line - can useage if required
      display.setCursor(0,8);
      display.println("BtS:");
      display.setCursor(25,8);
      display.println(buttonClick[2]);
      display.setCursor(31,8);
      display.println(buttonClick[1]);
      display.setCursor(37,8);
      display.println(buttonClick[0]);
      display.setCursor(49,8);
      display.println("R/L:");
      display.setCursor(72,8);
      display.println(digitalRead(RelayPin));
      display.setCursor(78,8);
      display.println(digitalRead(LedStripPin));
      
    }
  
    display.display();
  }  
}





String addleaderzero(int val)
{
  // add leading zero if < 10
  String addZero = "";
  String strVal = String(val);
  
  if ( val <=9 )
  {
    return "0" + strVal;
  }
  else
  {
    return strVal;
  }
}

void playMelody(int soundPin)
{
  int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
  };

  int noteDurations[] = {
    4, 8, 8, 4, 4, 4, 4, 4
  };

  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(soundPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(soundPin);
  }
}

void playMusicStarWars(void)
{
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

int counter = 0;

  beep(a, 500);
  beep(a, 500);    
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);  
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  delay(500);
 
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);  
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
//  delay(500);
 
}

void beep(int note, int duration)
{
  //Play tone on buzzerPin
  tone(SoundPin, note, duration);
 
  delay(duration);

  //Stop tone on buzzerPin
  noTone(SoundPin);
 
  delay(50);
 
  //counter++;
}

//long number1 = 123456789;
//long number2 = 987654321;

//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EEPROMWritelong(int address, long value)
       {
       //Decomposition from a long to 4 bytes by using bitshift.
       //One = Most significant -> Four = Least significant byte
       byte four = (value & 0xFF);
       byte three = ((value >> 8) & 0xFF);
       byte two = ((value >> 16) & 0xFF);
       byte one = ((value >> 24) & 0xFF);

       //Write the 4 bytes into the eeprom memory.
       EEPROM.write(address, four);
       EEPROM.write(address + 1, three);
       EEPROM.write(address + 2, two);
       EEPROM.write(address + 3, one);
       }

//This function will return a 4 byte (32bit) long from the eeprom
//at the specified address to address + 3.
long EEPROMReadlong(long address)
       {
       //Read the 4 bytes from the eeprom memory.
       long four = EEPROM.read(address);
       long three = EEPROM.read(address + 1);
       long two = EEPROM.read(address + 2);
       long one = EEPROM.read(address + 3);

       //Return the recomposed long by using bitshift.
       return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
       }
