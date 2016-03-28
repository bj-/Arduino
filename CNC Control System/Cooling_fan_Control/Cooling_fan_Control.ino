//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define ONE_WIRE_BUS 3

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


// Relay for cooling Fan
int RelayPin = 5;  // Relay Pin

// Spindle is Run
int SwitchPin = 4; // Input Switch


// iside Temperature index
bool OverHeat = false;

bool RelayPinState = false;
// Timer
int TimeFromStart = 0;

void setup()
{
  delay(1000);

  dht.begin();

  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  pinMode(RelayPin, OUTPUT);
  pinMode(SwitchPin, INPUT);
  
  Serial.begin(9600);
}


void loop()
{
  delay(1000);

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
    delay(1000);
    return;
  }

  // DS18B20
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  lcd.setCursor(0,0);
  lcd.print("Humidity:           ");
  lcd.setCursor(0,1);
  lcd.print("t in:     ou:     *C");
  lcd.setCursor(0,2);
  lcd.print("Sw:    t:    Rel:   ");

  lcd.setCursor(0,3);
  lcd.print(millis());

  // Humidity
  lcd.setCursor(10,0);
  lcd.print(h);
  // DHT temp
  lcd.setCursor(5,1);
  lcd.print(t_dht);
  // DS18B20tmp
  lcd.setCursor(13,1);
  lcd.print(t_ds);

  // SWwitch Pin State;
  int SwitchPinState = digitalRead(SwitchPin);
  bool SwitchStatus = (SwitchPinState == HIGH) ? false : true;
  String SwitchPinStatus = (SwitchPinState == HIGH) ? "OFF" : "ON";
  lcd.setCursor(3,2);
  lcd.print(SwitchPinStatus);

  // OverHeat
  //bool OverHeat = 
  if (t_dht >= (t_ds + 2))
  {
    OverHeat = true;
  }
  else
  {
    if (t_dht < (t_ds + 1))
    {
      OverHeat = false;
    }
  }
  String OverHeatStatus = (OverHeat) ? "YES" : "NO";
  lcd.setCursor(9,2);
  lcd.print(OverHeatStatus);

  // Timer Status

  String RelayPinStatus = (RelayPinState == true) ? "ON" : "OFF"; 
  lcd.setCursor(17,2);
  lcd.print(RelayPinStatus);

  
  // if (Temperature from outside sensor + 2) >= temperature inside box = Shiould run Cooling Fan
  if ( (t_dht >= (t_ds +2)) or digitalRead(SwitchPin) == LOW)
  {
    digitalWrite(RelayPin, HIGH);
    TimeFromStart = millis();
    //lcd.setCursor(0,2);
    //lcd.print("Switch: 1  Relay: 1 ");
    
  }
  else 
  {
    if (t_dht <= t_ds +0.5 and digitalRead(SwitchPin) == HIGH and (millis() > (TimeFromStart + 10000)) )
    {
      //lcd.setCursor(0,2);
      //lcd.print("Switch: 0  Relay: 0 ");
     
      digitalWrite(RelayPin, LOW);      
    }
  }

  /*
  // Switch
  if (SwitchPin == LOW)
  {
    RelayPin = HIGH;
  }
  else if ()
*/

  //lcd.setCursor(0,2);
  //lcd.print("Arduino LCM IIC 2004");
  // lcd.setCursor(2,3);
   
  //lcd.print("Power By Ec-yuan!");lcd.setCursor(0,0);

     //Serial.println("  !");
  /*
  lcd.print("WTF, world?");
  lcd.setCursor(1,0);
  lcd.print("TF, world?");
  lcd.setCursor(2,0);
  lcd.print("F, world?");
*/}
