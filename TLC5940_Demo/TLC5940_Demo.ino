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



  int direction = 1;
  for (int channel = 0; channel < NUM_TLCS * 16; channel += direction) {

    /* Tlc.clear() sets all the grayscale values to zero, but does not send
       them to the TLCs.  To actually send the data, call Tlc.update() */
    Tlc.clear();

    /* Tlc.set(channel (0-15), value (0-4095)) sets the grayscale value for
       one channel (15 is OUT15 on the first TLC, if multiple TLCs are daisy-
       chained, then channel = 16 would be OUT0 of the second TLC, etc.).

       value goes from off (0) to always on (4095).

       Like Tlc.clear(), this function only sets up the data, Tlc.update()
       will send the data. */
    if (channel == 0) {
      direction = 1;
    } else {
      Tlc.set(channel - 1, 1000);
    }
    Tlc.set(channel, 4095);
    if (channel != NUM_TLCS * 16 - 1) {
      Tlc.set(channel + 1, 1000);
    } else {
      direction = -1;
    }

    /* Tlc.update() sends the data to the TLCs.  This is when the LEDs will
       actually change. */
    Tlc.update();

    delay(75);
  }

}  
