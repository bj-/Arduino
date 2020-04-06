/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/
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


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

void setup() {
  Serial.begin(9600);

  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");


  // method 1: by index
  if (!sensors.getAddress(TopTSens, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(BottomTSens, 1)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(HotTSens, 2)) Serial.println("Unable to find address for Device 2");

    // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(TopTSens);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(BottomTSens);
  Serial.println();

  Serial.print("Device 2 Address: ");
  printAddress(HotTSens);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(TopTSens, TEMPERATURE_PRECISION);
  sensors.setResolution(BottomTSens, TEMPERATURE_PRECISION);

  //Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(TopTSens), DEC);
  Serial.println();

  //Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(BottomTSens), DEC);
  Serial.println();

  //Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(HotTSens), DEC);
  Serial.println();
  
  
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

  // Draw a single pixel in white
  //display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  //display.display();
  //delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  testdrawstyles();    // Draw 'stylized' characters
  // Invert and restore display, pausing in-between
  //display.invertDisplay(true);
  //display.invertDisplay(false);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("    32 C"));
  display.display();
  delay(2000);

}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("+                   +"));
  display.println();
  display.println(F("T                   T"));
  display.println(F("e                   i"));
  display.println(F("m                   m"));
  display.println(F("p                   e"));
  display.println();
  display.println(F("-                   -"));
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(1);             // Draw 2X-scale text
  display.println();
  display.setTextSize(2);             // Draw 2X-scale text
  display.println(F("    32 C"));
  display.setTextSize(1);             // Draw 2X-scale text
  display.println();
  display.setTextSize(2);             // Normal 1:1 pixel scale+
  display.println(F("   02:59"));
  display.setTextSize(1);             // Draw 2X-scale text
  display.println();
  display.println(F("         Run"));
  
//  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text

  display.display();
  delay(2000);
}
