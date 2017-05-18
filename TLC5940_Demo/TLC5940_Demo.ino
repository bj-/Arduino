/*
    Basic Pin setup (by BitBang protocol):
    ------------                                  ---u----
    ARDUINO   13|                           OUT1 |1     28| OUT channel 0
              12|                           OUT2 |2     27|-> GND (VPRG)
              11|                           OUT3 |3     26|-> SIN (pin 7)
              10|                           OUT4 |4     25|-> SCLK (pin 4)
               9|-> XLAT (pin 24)             .  |5     24|-> XLAT (pin 9)
               8|                             .  |6     23|-> BLANK (pin 6)
               7|-> SIN (pin 26)              .  |7     22|-> GND
               6|-> BLANK (pin 23)            .  |8     21|-> VCC (+5V)
               5|                             .  |9     20|-> 2K Resistor -> GND
               4|-> SCLK (pin 25)             .  |10    19|-> +5V (DCPRG)
               3|-> GSCLK (pin 18)            .  |11    18|-> GSCLK (pin 3)
               2|                             .  |12    17|-> SOUT
               1|                             .  |13    16|-> XERR
               0|                           OUT14|14    15| OUT channel 15
    ------------                                  --------

*/

#include "Tlc5940.h"

void setup()
{
   Tlc.init();  

   // When SS Pin is moved to D6
   pinMode(6, OUTPUT);
   analogWrite(6, 1);
}
 
unsigned int br = 4095;  
 
void loop()  
{
  for (byte i=0; i < NUM_TLCS * 16; i++)
  {
    Tlc.set(i, br);
  }
  Tlc.update();
  delay(1000);
  if (br==0) br = 4095; else br=0;  
  //if (br==0) br = 4095; else br=4095;  




}  
