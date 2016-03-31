//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define ONE_WIRE_BUS 3    // DS18B20 Pin

int RelayPin = 5;  // Relay Pin (Relay for cooling Fan)
int SwitchPin = 4; // Input Switch (Spindle is Run)

int RunDelay = 300; // Delay in seconds after last change state of input switch or t Alarm before off cooling fan FAN
int TemperatureShift = 5; // Shift of temperature. Run FUN when inside temperature more than outside.


// =============================
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// iside Temperature index
bool OverHeat = false;

bool RelayPinState = false;

// Timer
//int TimeFromStart = 0;
// Changed state (OverHeat is ON / Switch input is ON
// Used for Timer (delay before off FAN)
bool ChangedState = false;
long ChangedStateTime = 0;
bool TimerState = false;
int CountDown = 0;

float uptime = 0;

void setup()
{

  dht.begin();

  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  // Default Screen
  lcd.setCursor(0,0);
  lcd.print("DHT:      %       *C");
  lcd.setCursor(0,1);
  lcd.print("DS:      *C; tOn:+  ");
  lcd.setCursor(18,1);
  lcd.print(TemperatureShift);
  lcd.setCursor(0,2);
  lcd.print("Sw:  t:   Tm:    R: ");
  lcd.setCursor(0,3);
  lcd.print("UT:            v:1.1");
  
  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  pinMode(RelayPin, OUTPUT);
  pinMode(SwitchPin, INPUT);
  
  Serial.begin(9600);
  Serial.println("CNC Monitoring System");
  Serial.println("Cooling FAN Control module");
  Serial.println("v: 1.1");
}


void loop()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  // from DHT
  float t_dht = dht.readTemperature();
  // DROM DS 18B20
  float t_ds = sensors.getTempCByIndex(0);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t_dht)) {
    lcd.setCursor(0,0);
    lcd.print("Failed to read");
    lcd.setCursor(0,1);
    lcd.print("from DHT sensor!");
    delay(2000);
    return;
  }

  // DS18B20 read temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  

  // Uptime
  uptime = (millis()/1000);
  lcd.setCursor(3,3);
  lcd.print(uptime,0);

  Serial.print("Uptime: ");
  Serial.print(uptime);

  // Humidity
  lcd.setCursor(4,0);
  lcd.print(h);
  // DHT temp
  lcd.setCursor(12,0);
  lcd.print(t_dht);
  // DS18B20tmp
  lcd.setCursor(3,1);
  lcd.print(t_ds);

  Serial.print("DHT Humidity: ");
  Serial.print(h);
  Serial.print(" / Temp (*C): ");
  Serial.print(t_dht);
  Serial.print("; DS18B20 temp (*C): ");
  Serial.print(t_ds);

  lcd.setCursor(0,2);
  lcd.print("Sw:  t:   Tm:    R: ");
  
  // SWwitch Pin State;
  bool SwitchStatus = false;
  if ( digitalRead(SwitchPin) == LOW )
  {
    SwitchStatus = true;
    ChangedStateTime = millis()/1000;
  }

  Serial.print("; Switch input status: ");
  Serial.print(SwitchStatus);
  
  lcd.setCursor(3,2);
  lcd.print(SwitchStatus);

  // OverHeat
  if (t_dht >= (t_ds + TemperatureShift))
  {
    OverHeat = true; // true - FAN Should be RUN
    ChangedStateTime = millis()/1000;
  }
  else if (t_dht < (t_ds + 1))
  {
    OverHeat = false;
  }
  else
  {
    OverHeat = false;
  }
  String OverHeatStatus = (OverHeat) ? "Y" : "N";
  lcd.setCursor(7,2);
  lcd.print(OverHeatStatus);
  lcd.setCursor(8,2);
  lcd.print((t_dht-t_ds),0);

  // Timer Status
  Serial.print("; t Alarm/Switch is Active: ");
  Serial.print(ChangedState);
  Serial.print("; t different (DHT-DS): ");
  Serial.print((t_dht-t_ds),2);
  int CountSec = millis()/1000 - ChangedStateTime;


  if (CountSec < RunDelay)
  {
    CountDown = RunDelay - CountSec;
  }
  else 
  {
    CountDown = 0;
  }

  Serial.print("; CountDown to OFF: ");
  Serial.print(CountDown);

  lcd.setCursor(13,2);
  lcd.print(CountDown);


  String RelayPinStatus = (CountDown > 0) ? "1" : "0"; 
  lcd.setCursor(19,2);
  lcd.print(RelayPinStatus);

  Serial.print("; Relay is: ");
  
  if (CountDown > 0)
  {
    digitalWrite(RelayPin, HIGH);
    Serial.println("ON");
  }
  else 
  {
    digitalWrite(RelayPin, LOW);      
    Serial.println("OFF");
  }
}
