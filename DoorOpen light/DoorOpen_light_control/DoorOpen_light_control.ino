
// OptoSwitch #1
int OptoSw1 = A0;
int OptoSw2 = A3;
//int OptoSw1Read = 0;
// Test Led
int LedStrip = 9;
int testLed1 = 13;

bool LedStatus = false; // ON / OFF

void setup() {
  pinMode(OptoSw1, INPUT);
  pinMode(OptoSw2, INPUT);
  pinMode(testLed1, OUTPUT);

  // выключаем тестовый светодиод
  digitalWrite(testLed1, LOW);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  int ReadVal1 = 0;

  for (int i=0; i <= 10; i++){
    ReadVal1 = ReadVal1 + analogRead(OptoSw1);
    delay(10);
  }
  ReadVal1 = ReadVal1 / 11;

  int ReadVal2 = 0;

  for (int i=0; i <= 10; i++){
    ReadVal2 = ReadVal2 + analogRead(OptoSw2);
    delay(10);
  }
  ReadVal2 = ReadVal2 / 11;
/* */

  Serial.print("Hall: ");
  Serial.println(ReadVal1);
  Serial.print(", opto: ");
  Serial.println(ReadVal2);
  delay(1);


/*
  digitalWrite(testLed1, LOW);
      delay(1000);

  digitalWrite(testLed1, HIGH);
    delay(1000);
*/
  if (ReadVal1 > 100)
  {
    if (LedStatus != true) 
    {
      digitalWrite(testLed1, HIGH);

      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
      }
      // после включения 10 секунд ждем безусловно. потом только начинаем пасти происходящее
      delay(1000);

    }
    LedStatus = true;
//*/
  }
  if (ReadVal1 < 10) 
  {
    // fade out from max to min in increments of 5 points:

    if (LedStatus != false) 
    {
      digitalWrite(testLed1, LOW);
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        //Serial.println(fadeValue);
        delay(10);
      }
    }
    LedStatus = false;
//*/
  }
}
