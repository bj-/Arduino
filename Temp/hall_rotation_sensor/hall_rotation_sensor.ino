// read RPM

volatile int rpmcount = 0;//see http://arduino.cc/en/Reference/Volatile
 int rpm = 0;
 unsigned long lastmillis = 0;

void setup(){
 Serial.begin(9600);
 attachInterrupt(0, rpm_fan, FALLING);//interrupt cero (0) is on pin two(2).
 }

void loop(){

 if (millis() - lastmillis == 1000){ /*Uptade every one second, this will be equal to reading frecuency (Hz).*/

 detachInterrupt(0); //Disable interrupt when calculating

 rpm = rpmcount * 60; /* Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use rpmcount * 30.*/

 Serial.print("RPM =\t"); //print the word "RPM" and tab.
 Serial.print(rpm); // print the rpm value.
 Serial.print("\t Hz=\t"); //print the word "Hz".
 Serial.println(rpmcount); /*print revolutions per second or Hz. And print new line or enter.*/

 rpmcount = 0; // Restart the RPM counter
 lastmillis = millis(); // Uptade lasmillis
 attachInterrupt(0, rpm_fan, FALLING); //enable interrupt
 }
 }

void rpm_fan(){ /* this code will be executed every time the interrupt 0 (pin2) gets low.*/
 rpmcount++;
 }

