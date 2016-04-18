/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"
#include <DS1302.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define DHTPIN 9     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// Init the DS1302
DS1302 rtc(6, 7, 8);
// Init a Time-data structure
Time t;

int OnTimeH = 10;
int OnTimeM = 00;
int OffTimeH = 22;
int OffTimeM = 00;


int RelayPin = 10;

String RelayStatus = "Off";

void setup() {
  
  dht.begin();
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 2);

  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);

   Serial.begin(9600);

   pinMode(RelayPin, OUTPUT);

// The following lines can be commented out to use the values already stored in the DS1302

/*
//  rtc.setDOW(SATURDAY);        // Set Day-of-Week to FRIDAY
  rtc.setDOW(WEDNESDAY);        // Set Day-of-Week to FRIDAY
  rtc.setTime(07, 58, 40);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(13, 04, 2016);   // Set the date to August 6th, 2010
//*/
}

void loop() {

  t = rtc.getTime();

  //lcd.setCursor(0, 0);
  //lcd.print(t.hour, DEC);
//Serial.println(t);

  // Clear display
  lcd.setCursor(0, 0);
  lcd.print("        ");
  lcd.setCursor(0, 1);
  lcd.print("        ");


  lcd.setCursor(0, 0);
  lcd.print(t.hour, DEC);
  lcd.setCursor(2, 0);
  lcd.print(":");
  lcd.setCursor(3, 0);
  lcd.print(t.min, DEC);
  lcd.setCursor(5, 0);
  lcd.print(":");
  lcd.setCursor(6, 0);
  lcd.print(t.sec, DEC);


  // Send Day-of-Week and time
  Serial.print("It is the ");
  Serial.print(t.dow, DEC);
  Serial.print(". day of the week ");
  Serial.print(t.hour, DEC);
  Serial.print(":");
  Serial.print(t.min, DEC);
  Serial.print(":");
  Serial.print(t.sec, DEC);
  Serial.println(";");

 
  lcd.setCursor(0, 1);
  lcd.print(RelayStatus);
  lcd.setCursor(3, 1);
  lcd.print("10-22");

  if (t.hour >= OnTimeH & t.min >= OnTimeM)
  {
     if (t.hour >= OffTimeH & t.min >= OffTimeM )
     {
       digitalWrite(RelayPin, LOW);
       RelayStatus = "Off";
     }
     else
     {
       digitalWrite(RelayPin, HIGH);
       RelayStatus = "On";
     }
  }
  else 
  {
     digitalWrite(RelayPin, LOW);
     RelayStatus = "Off";
  }

delay(3000);
/*
  Serial.print(t.dow, DEC);
  Serial.print(". day of the week (counting monday as the 1th), and it has passed ");
  Serial.print(t.hour, DEC);
  Serial.print(" hour(s), ");
  Serial.print(t.min, DEC);
  Serial.print(" minute(s) and ");
  Serial.print(t.sec, DEC);
*/  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  if (isnan(t) || isnan(h)) 
  {
    lcd.setCursor(0, 0);
    lcd.print("DHT sens");
    lcd.setCursor(0, 1);
    lcd.print("missed  ");
  } 
  else 
  {
    // Print a message to the LCD.
    lcd.setCursor(0, 0);
    lcd.print("   * C  ");
    lcd.setCursor(0, 1);
    lcd.print("   % RH ");

    lcd.setCursor(0, 0);
    lcd.print(t);
    lcd.setCursor(0, 1);
    lcd.print(h);
  }

  delay(5000);

}

