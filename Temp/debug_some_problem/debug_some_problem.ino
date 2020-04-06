// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dallas Temp Sensor Init
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress TopTSens, BottomTSens, HotTSens;

// Display Init
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Assign address manually. The addresses below will beed to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
// DeviceAddress TopTSens = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress BottomTSens   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

// Pins
const int SoundPin = 4;
int LedPin = 11;
int RelayPin = 10;

const int buttonsCnt = 5;
int buttonPin[buttonsCnt] = {9, 5, 6, 7, 8}; //Display buttons: Right - Center - Left
bool buttonClick[buttonsCnt] = {false, false, false, false, false};
bool ignoreButton[buttonsCnt] = {false, false, false, false, false};
//bool buttonLight[buttonsCnt] = {false, false, false, false, false}; // подсветка нажатой кнопки
int buttonTicCount[buttonsCnt] = {0, 0, 0, 0, 0};
#define TIME_BUTTON 3            // время устойчивого состояния кнопки (* +-2 мс) 

long timer = 3600; // Timer in seconds (Start Value)
const int timerPresetsSIZE = 13;
long timerProgPresets[timerPresetsSIZE] = {60, 30*60, 40*60, 50*60, 1*60*60, 1*90*60, 2*60*60, 3*60*60, 5*60*60, 8*60*60, 10*60*60, 12*60*60, 24*60*60}; // Timer in seconds (presets)

unsigned long startTime = 0;  //When count down timer stared
String addZero = ""; // if minutes or sec0nds less than 10 - add 0 before value
bool timerWorking = false;
//int timeToDown = 2;
unsigned long timeToDown = 1;
int timeToDownHours = 0;
int timeToDownMinutes = 0;
int timeToDownSeconds = 0;

bool noMusic = false;
bool Debug = false;
bool inMenu = false; // Если есть меню то у кнопок может быть другой режим работвы.

int temperature = 33;

void setup(void)
{

  // Set Button pins mode
  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
  //  Serial.println(buttonPin[i]);
    pinMode(buttonPin[i], INPUT);
  }
  pinMode(LedPin, OUTPUT);

  // start serial port
  Serial.begin(9600);
//  Serial.println("Dallas Temperature IC Control Library Demo");

  


  // Display Init
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  //delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

}




/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();
  // print the device information
  float TempTop = 1;
  float TempBtm = 1;
  float TempHot = 1;
  if ( 1==1) {
        Serial.println("dfsgdsfgdfgBujhRejlhjhjay = swsgitch on");
  }
  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
    ReadButtonState (i);
  }

    // Start Timer and switch relay
    if ( buttonClick[4] == true and timerWorking == false and ignoreButton[4] == false and inMenu == false )
    {
      timerWorking = true;
      startTime = millis()/1000;
      digitalWrite(RelayPin, HIGH);
      ignoreButton[4] = true;
      noMusic = false; // reset to default
      Serial.println("Button5 = click, Relay = switch on");

    }
    else if ( buttonClick[4] == true and timerWorking == true and ignoreButton[4] == false and inMenu == false )
    {
      timerWorking = false;
      digitalWrite(RelayPin, LOW);
      ignoreButton[4] = true;
      noMusic = true; // do not play music if stopped manualy
      Serial.println("Button5 = click, Relay = switch OFF");
    }

  // Ajust Timer
  if ( buttonClick[2] == true and ignoreButton[2] == false and inMenu == false )
  {
    if ( timerWorking )
    {
      timer = timer + 60;
      if ( buttonTicCount[2] > 40 ) { timer = timer + 60*10; }
      else if ( buttonTicCount[2] > 15 ) { timer = timer + 60*2; }
    
      if ( timer > 59999 ) { timer = 59999; }
      
    }
    else
    {
      for (int t = 0; t < timerPresetsSIZE; t++) 
      {
        Serial.print(t);

        long x = timerProgPresets[t];
        if ( x > timer )
        { 
          Serial.print(F("-|"));
          if ( 2 <= 9 )
          {
            //String strVal = String(2);
            //strVal = "f0" + strVal;
            //Serial.print(F("f0"));
          }
          //Serial.print(addleaderzero(2));;
          Serial.print(F("|-"));
          Serial.print(timer);
          Serial.print(F("-"));
          Serial.print(timerProgPresets[t]);
          Serial.print(F("-"));
          timer = timerProgPresets[t]; 
          t = timerPresetsSIZE;
        }
      }
    }
  }
  else if ( buttonClick[3] == true and ignoreButton[3] == false and inMenu == false )
  {
    if (timerWorking)
    {
      timer = timer - 60;
      if ( buttonTicCount[3] > 40 ) timer = timer - 60*10;
      else if ( buttonTicCount[3] > 15 ) timer = timer - 60*2;

      if (timer < 60 ) timer = 60;
    }
    else
    {
      //for (i=0; i < timerPresetsSIZE; i++)
      //{
        //if (timerPresets[i] > timer and i > 0) timer = timerPresets[i-1];
      //}

    }
  }

  // Ajust Temperature
  if ( buttonClick[0] == true and ignoreButton[0] == false and inMenu == false )
  {
    temperature = temperature + 1;
    if ( temperature > 60 ) temperature = 60;

  }
  else if ( buttonClick[1] == true and ignoreButton[1] == false and inMenu == false )
  {
    temperature = temperature - 1;
    if (temperature < 10 ) temperature = 10;
  }
  

    
    Serial.print("=");
    Serial.print(timer);
    Serial.print("=");
    
    if ( timerWorking == true )
    {
      timeToDown=timer - ( millis()/1000 - startTime );

      timeToDownHours=timeToDown/3600;
      timeToDownMinutes=(timeToDown/60)%60;
      timeToDownSeconds=timeToDown%60;
        //timeToDownMinutes=timeToDown/60;
      //timeToDownSeconds=timeToDown%60;

      if ( timeToDown < 1 )
      {
        timerWorking = false;
        digitalWrite(RelayPin, LOW);
        digitalWrite(LedPin, HIGH);
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
//      timeToDownMinutes=timer/60;
//      timeToDownSeconds=timer%60;
      timeToDownHours=timer/3600;
      timeToDownMinutes=(timer/60)%60;
      timeToDownSeconds=timer%60;
    }
    Serial.print(timeToDownHours);
    Serial.print(F(":"));
    Serial.print(timeToDownMinutes);
    Serial.print(F(":"));
    Serial.print(timeToDownSeconds);
    Serial.print(F("="));


/*
    // Start Timer and switch relay
    if ( buttonClick[2] == true and timerWorking == false and ignoreButton[2] == false and inMenu == false )
    {
      timerWorking = true;
      startTime = millis()/1000;
      digitalWrite(RelayPin, HIGH);
      ignoreButton[2] = true;
      noMusic = false; // reset to default
      Serial.println(F("Button3 = click, Relay = switch on"));

    }
    else if ( buttonClick[2] == true and timerWorking == true and ignoreButton[2] == false and inMenu == false )
    {
      timerWorking = false;
      digitalWrite(RelayPin, LOW);
      ignoreButton[2] = true;
      noMusic = true; // do not play music if stopped manualy
      Serial.println(F("Button3 = click, Relay = switch OFF"));
    }
*/
  //digitalWrite(LedPin, HIGH);

  Serial.print(F("Temperatures: "));
  Serial.print(F("Top: "));
  Serial.print(TempTop);
  Serial.print(F(";  Botom: "));
  Serial.print(TempBtm);
  Serial.print(F(";  HotPlate: "));
  Serial.print(TempHot);
  Serial.print(F("; buttons: "));
  Serial.print(buttonClick[0]);
  Serial.print(buttonClick[1]);
  Serial.print(buttonClick[2]);
  Serial.print(buttonClick[3]);
  Serial.print(buttonClick[4]);
  Serial.println(F("; DONE"));



  //Display
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  // Menu
  display.setCursor(1,0);             // " +  Temp - "
  display.println(F("+"));
  display.println();
  display.println(F("T"));
  display.println(F("e"));
  display.println(F("m"));
  display.println(F("p"));
  display.println();
  display.setCursor(1,57);
  display.println(F("-"));

  display.setCursor(120,0);             // " +  Time - "
  display.println(F("+"));
  display.setCursor(120,15);
  display.println(F("T"));
  display.setCursor(120,23);
  display.println(F("i"));
  display.setCursor(120,32);
  display.println(F("m"));
  display.setCursor(120,40);
  display.println(F("e"));
  display.setCursor(120,57);
  display.println(F("-"));

  display.setCursor(45,57);             // "  Run  " / "   Stop   "
  if ( timerWorking )
  {
    display.println(F("Stop"));
  }
  else
  {
    display.println(F("Run"));
  }

  if ( buttonTicCount[0] >= 1 ) { display.fillRect(0, 0, 7, 7, SSD1306_INVERSE); }
  if ( buttonTicCount[1] >= 1 ) { display.fillRect(0, 57, 7, 7, SSD1306_INVERSE); }
  if ( buttonTicCount[2] >= 1 ) { display.fillRect(119, 0, 7, 7, SSD1306_INVERSE); }
  if ( buttonTicCount[3] >= 1 ) { display.fillRect(119, 57, 7, 7, SSD1306_INVERSE); }
  if ( buttonTicCount[4] >= 1 ) { display.fillRect(40, 56, 35, 8, SSD1306_INVERSE); }

//  if ( buttonTicCount[4] == 1 ) { buttonLight[4] = true; display.fillRect(50, 56, 22, 8, SSD1306_INVERSE); }
//  else if ( buttonTicCount[4] == 0 && buttonLight[4] == true ) { buttonLight[4] = false; display.fillRect(50, 56, 22, 8, SSD1306_INVERSE); }

  display.setTextSize(1);             // Debug Info (Temp by sensors)
  //display.setTextColor(SSD1306_WHITE);
  display.setCursor(12,0);
  //display.print(F("+"));
  display.print(TempTop);
  display.print(F("/"));
  display.print(TempBtm);
  display.print(F("/"));
  display.print(TempHot);
  //display.println();


  display.setTextSize(2);             // Draw 2X-scale text
  display.setCursor(41,14);             // Temp
  display.print(temperature);
  display.print(F(" C"));
  display.setCursor(32,35);             // Countdown Timer

  Serial.print(F("="));
  Serial.print(timeToDownHours);
  Serial.print(F("."));
  Serial.print(addleaderzero(timeToDownMinutes));
  Serial.print(F("."));
  Serial.print(addleaderzero(timeToDownSeconds));
  Serial.print(F("."));
  Serial.print(addleaderzero(3));
  Serial.print(F("="));
    
  display.setTextSize(2);
  display.setCursor(28,35);
  //display.setCursor(13,15);
  //display.print(addleaderzero(timeToDownHours));
  display.print(addleaderzero(timeToDownHours));
  display.print(F(":"));
  display.print(addleaderzero(timeToDownMinutes));
  display.setTextSize(1);
  display.print(F(":"));
  display.print(addleaderzero(timeToDownSeconds));
    
  //display.println(F("02:58"));
  
//  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.display();
  //delay(2000);

  
  //display.fillRect(8, 0, 111, 7, SSD1306_BLACK);

    //delay(50);
    
  // printData(TopTSens);
  // printData(BottomTSens);
  // printData(HotTSens);
}
