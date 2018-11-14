#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

//initialize LCD with the numbers of the interface pins
//    LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int start;
int lm;
int procent;

//Переменные из люксметра от samopal.pro
#define MAX_UNITS 12 
uint16_t l_min=0,l_max=0,l=0,env=0;
int pulse = 0;
   
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

LiquidCrystal_I2C lcd(0x27,16,2);

void configureSensor(void)
{
  tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  // tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

}

void setup(void) 

{
// set up the LCD's number of columns and rows:
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  // Default Screen
  lcd.setCursor(0,0);
  lcd.print("Curr:           ");
  
  
  Serial.begin(9600);
  Serial.println("Lamtest welcome...");
  /* Initialise the sensor */
//delay(5000);
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  /* Setup the sensor gain and integration time */
  configureSensor();

// Записываем исходное значение
   
  sensors_event_t event;
  tsl.getEvent(&event);
  start = event.light;
 
lcd.setCursor(0,1);
lcd.print(start);

  Serial.print("start:");
  Serial.println(start);
}

void loop(void) 
{  
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS); 
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
lm = event.light;

lcd.setCursor(0,0);
lcd.print(lm); lcd.print("  ");

procent = (event.light/start*100);

lcd.setCursor(10,0);
lcd.print(procent); lcd.print("%  ");

{
      int x;
      x = analogRead (0);
      
      if (x < 60) {
//        lcd.print ("Right ");
      }
      else if (x < 200) {
//        lcd.print ("Up    ");
      }
      else if (x < 400){
//        lcd.print ("Down  ");
      }
      else if (x < 600){
//        lcd.print ("Left  ");
env = event.light;
      }
      else if (x < 800){
start = lm;     
lcd.setCursor(0,1);
lcd.print(start);
lcd.print("    ");
      }
}

// Замер пульсации 
tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

l=event.light-env;
l_min = l;
l_max = l;
  
  for (int i = 0; i < MAX_UNITS; i = i + 1) {

   tsl.getEvent(&event);

l=event.light-env;   
if( l < l_min )l_min = l;
if( l > l_max )l_max = l;
}
      if( l_max != 0 )pulse = (double)((l_max - l_min))*100/(double)((l_max + l_min));
      else pulse = 0; 
   
lcd.setCursor(10,1);
lcd.print(pulse);
lcd.print("    ");
}
