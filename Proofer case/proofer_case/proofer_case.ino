// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastPID.h>

                                              //
                                              //      ATTENTION !!!!
                                              //   Maximum can use less than 790 bytes of dynamic memory
                                              //   didn't work correctly if more
                                              //
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


// PID
float Kp=0.1, Ki=0.5, Kd=0, Hz=10;
int output_bits = 8;
bool output_signed = false;
FastPID myPID(Kp, Ki, Kd, Hz, output_bits, output_signed);
uint8_t output2 = 0;


// Display Init
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16


// Pins
const int SoundPin = 4;
#define LED_PIN       11
#define RELAY_PIN     10

// Variables
const int buttonsCnt = 5;
int buttonPin[buttonsCnt] = {9, 5, 6, 7, 8};            //Display buttons: Right - Center - Left
bool buttonClick[buttonsCnt] = {false, false, false, false, false};     // button current state
bool ignoreButton[buttonsCnt] = {false, false, false, false, false};    // button ignoge if pressed current state
int buttonTicCount[buttonsCnt] = {0, 0, 0, 0, 0};                       // button pressed timer
#define TIME_BUTTON 1            // время устойчивого состояния кнопки (* +-2 мс) 
bool OverHeat = false;

// Timer
unsigned long timer = 3600;     // Default Timer in seconds (Start Value)
const int timerPresetsSIZE = 11;
unsigned long timerProgPresets[timerPresetsSIZE] = {60, 30*60, 40*60, 50*60, 1*60*60, 1.5*60*60, 2*60*60, 3*60*60, 5*60*60, 7*60*60, 9*60*60}; // Timer in seconds (presets)
//unsigned long timerProgPresets[timerPresetsSIZE] = {60, 30*60, 40*60, 50*60, 1*60*60, 1.5*60*60, 2*60*60, 3*60*60, 5*60*60, 7*60*60, 9*60*60, 12*60*60, 24*60*60}; // Timer in seconds (presets)

unsigned long startTime = 0;  //When count down timer stared
//String addZero = "";          // if minutes or sec0nds less than 10 - add 0 before value
bool timerWorking = false;      // Таймер работает: да / нет
//int timeToDown = 2;
unsigned long timeToDown = 1;   // Скорость уменьшения таймера - 1 = 1 сек
int timeToDownHours = 0;        // для вывода на экран. таймер раскладывается на часы / минуты / секунды
int timeToDownMinutes = 0;
int timeToDownSeconds = 0;

bool noMusic = false;
bool Debug = false;   // Дебаг режим (пишем в порт всякую фигню и на эран)
//bool Debug = true;   // Дебаг режим (пишем в порт всякую фигню и на эран)
bool inMenu = false; // Если есть меню то у кнопок может быть другой режим работвы.

int temperature = 33;   // стартовая дефолтная температура
#define MAX_HOT_PLATE_TEMPERATURE    60

void setup(void)
{

  // Set pins mode
  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
  //  Serial.println(buttonPin[i]);
    pinMode(buttonPin[i], INPUT);
  }
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);


  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();

  //if (Debug)
  //{
    // locate devices on the bus
    Serial.print(F("Locating devices..."));
    Serial.print(F("Found "));
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(F(" devices."));

    // report parasite power requirements
    Serial.print(F("Parasite power is: "));
    if (sensors.isParasitePowerMode()) Serial.println(F("ON"));
    else Serial.println(F("OFF"));
    // method 1: by index
    if (!sensors.getAddress(TopTSens, 0)) Serial.println(F("Unable to find address for Device 0"));
    if (!sensors.getAddress(BottomTSens, 1)) Serial.println(F("Unable to find address for Device 1"));
    if (!sensors.getAddress(HotTSens, 2)) Serial.println(F("Unable to find address for Device 2"));

    // show the addresses we found on the bus
    Serial.print(F("Device 0 Address: "));
    printAddress(TopTSens);
    Serial.println();

    Serial.print(F("Device 1 Address: "));
    printAddress(BottomTSens);
    Serial.println();

    Serial.print(F("Device 2 Address: "));
    printAddress(HotTSens);
    Serial.println();
  //}

  // set the resolution to 9 bit per device
  sensors.setResolution(TopTSens, TEMPERATURE_PRECISION);
  sensors.setResolution(BottomTSens, TEMPERATURE_PRECISION);

  //if (Debug)
  //{
    Serial.print(F("Device 0 Resolution: "));
    Serial.print(sensors.getResolution(TopTSens), DEC);
    Serial.println();

    Serial.print(F("Device 1 Resolution: "));
    Serial.print(sensors.getResolution(BottomTSens), DEC);
    Serial.println();

    Serial.print(F("Device 2 Resolution: "));
    Serial.print(sensors.getResolution(HotTSens), DEC);
    Serial.println();
  //}

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
  float TempTop = sensors.getTempC(TopTSens);
  float TempBtm = sensors.getTempC(BottomTSens);
  float TempHot = sensors.getTempC(HotTSens);

  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
    ReadButtonState (i);
  }

    // Start Timer and switch relay
    if ( buttonClick[4] == true and timerWorking == false and ignoreButton[4] == false and inMenu == false )
    {
      timerWorking = true;
      startTime = millis()/1000;
      //digitalWrite(RELAY_PIN, HIGH);
      ignoreButton[4] = true;
      noMusic = false; // reset to default
      Serial.println(F("Button5 = click, Relay = switch on"));

    }
    else if ( buttonClick[4] == true and timerWorking == true and ignoreButton[4] == false and inMenu == false )
    {
      timerWorking = false;
      //digitalWrite(RELAY_PIN, LOW);
      analogWrite(RELAY_PIN, 0);
      ignoreButton[4] = true;
      noMusic = true; // do not play music if stopped manualy
      Serial.println(F("Button5 = click, Relay = switch OFF"));
    }

  // Ajust Timer
  if ( buttonClick[2] == true and ignoreButton[2] == false and inMenu == false )
  {
    if ( timerWorking or timer >= 9*60*60 ) //TODO Костыль: почему-то из массива берется мусор если число больше половины от 65535, даже при unsigned long типе
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
        if ( timerProgPresets[t] > timer )
        { 
          timer = timerProgPresets[t]; 
          t = timerPresetsSIZE;
        }
      }
    }
  }
  else if ( buttonClick[3] == true and ignoreButton[3] == false and inMenu == false )
  {
    if (timerWorking  or timer > 9*60*60 ) // TODO Зеркальынй костыль для уменьшения таймера свыше 65535/2 сек
    {
      timer = timer - 60;
      if ( buttonTicCount[3] > 40 ) timer = timer - 60*10;
      else if ( buttonTicCount[3] > 15 ) timer = timer - 60*2;

      if (timer < 60 ) timer = 60;
    }
    else
    {
      for (i=0; i < timerPresetsSIZE; i++)
      {
        if (timerProgPresets[i] >= timer and i > 0)
        {
          timer = timerProgPresets[i-1];
          i = timerPresetsSIZE;
        }
      }
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
  
    
  if ( timerWorking == true )
  {
    timeToDown=timer - ( millis()/1000 - startTime );

    timeToDownHours=timeToDown/3600;
    timeToDownMinutes=(timeToDown/60)%60;
    timeToDownSeconds=timeToDown%60;
    //timeToDownMinutes=timeToDown/60;
    //timeToDownSeconds=timeToDown%60;

    int setpoint = temperature; 
    int feedback = (TempTop+TempBtm)/2;
    uint32_t before, after;
    before = micros();
    uint8_t output = myPID.step(setpoint, feedback);
    after = micros();
  
    analogWrite(RELAY_PIN, output);
    
    // When tSensor did not work or hot plate overheat
    if ( TempHot > MAX_HOT_PLATE_TEMPERATURE or TempHot < -10 )
    {
      //analogWrite(RELAY_PIN, 0);
      output = 0;
      //      OverHeat = true;
    }

    
    analogWrite(RELAY_PIN, output);

    //OverHeat = () ? true : false;
    output2 = output;
  
    Serial.print(F("runtime: ")); 
    Serial.print(after - before);
    Serial.print(F(" sp: ")); 
    Serial.print(setpoint); 
    Serial.print(F(" fb: ")); 
    Serial.print(feedback);
    Serial.print(F(" out: "));
    Serial.println(output);

    if ( timeToDown < 1 )
    {
      timerWorking = false;
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, HIGH);
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


  
  //digitalWrite(LedPin, HIGH);
  
  if (Debug)
  {
    Serial.print(timeToDownHours);
    Serial.print(F(":"));
    Serial.print(timeToDownMinutes);
    Serial.print(F(":"));
    Serial.print(timeToDownSeconds);
    Serial.print(F("="));
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
  }





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
  display.setCursor(120,24);
  display.println(F("i"));
  display.setCursor(120,32);
  display.println(F("m"));
  display.setCursor(120,40);
  display.println(F("e"));
  display.setCursor(120,57);
  display.println(F("-"));

  display.setCursor(52,57);             // "  Run  " / "   Stop   "
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

  /*
  Serial.print(F("="));
  Serial.print(timeToDownHours);
  Serial.print(F("."));
  Serial.print(addleaderzero(timeToDownMinutes));
  Serial.print(F("."));
  Serial.print(addleaderzero(timeToDownSeconds));
  Serial.print(F("."));
  Serial.print(addleaderzero(3));
  Serial.print(F("="));
  */
      
  display.setTextSize(2);
  display.setCursor(28,35);
  display.print(addleaderzero(timeToDownHours));
  display.print(F(":"));
  display.print(addleaderzero(timeToDownMinutes));
  display.setTextSize(1);
  display.print(F(":"));
  display.print(addleaderzero(timeToDownSeconds));

  display.setCursor(13,56);
  display.setTextSize(1);
  if ( timerWorking ) 
  {
    if ( TempHot > MAX_HOT_PLATE_TEMPERATURE )
    {
      display.print(F("OvrHt"));
      display.fillRect(12, 55, 31, 8, SSD1306_INVERSE);
    }
    else
    {
      display.print(F("H"));
      display.print(output2);
    }
    if ( TempHot < -10 )
    {
      display.fillRect(90, 0, 30, 7, SSD1306_INVERSE);
    }
    //*/
  }


  //  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.display();
  //delay(2000);

  
  //display.fillRect(8, 0, 111, 7, SSD1306_BLACK);

    //delay(50);
    
  // printData(TopTSens);
  // printData(BottomTSens);
  // printData(HotTSens);
}
