
// OptoSwitch #1
int SW1 = 8;
int SW2 = 7;
//int OptoSw1Read = 0;
// Test Led
int LedStrip = 9;
int LedStrip2 = 10;
int testLed1 = 13;

bool LedStatus = false; // ON / OFF
bool LedStatus2 = false; // ON / OFF

void setup() {
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(testLed1, OUTPUT);
  pinMode(LedStrip, OUTPUT);
  pinMode(LedStrip2, OUTPUT);
  

  // выключаем тестовый светодиод
  digitalWrite(testLed1, LOW);
  digitalWrite(LedStrip, LOW);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  int ReadSW1 = 0;
  ReadSW1 = digitalRead(SW1);
  delay(10);

  int ReadSW2 = 0;
  ReadSW2 = digitalRead(SW2);
  delay(10);

  Serial.print("Hall-1: ");
  Serial.print(ReadSW1);
  Serial.print(", Hall-2: ");
  Serial.println(ReadSW2);
  delay(1);


/*
  digitalWrite(testLed1, LOW);
      delay(1000);

  digitalWrite(testLed1, HIGH);
    delay(1000);
*/
  if (ReadSW1 == 0)
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
      delay(10);

    }
    LedStatus = true;
//*/
  }
  if (ReadSW1 == 1) 
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
  if (ReadSW2 == 0)
  {
    if (LedStatus2 != true) 
    {
      digitalWrite(testLed1, HIGH);

      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip2, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
      }
      // после включения 10 секунд ждем безусловно. потом только начинаем пасти происходящее
      delay(10);

    }
    LedStatus2 = true;
//*/
  }
  if (ReadSW2 == 1) 
  {
    // fade out from max to min in increments of 5 points:
    if (LedStatus2 != false) 
    {
      digitalWrite(testLed1, LOW);
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 1) 
      {
        // sets the value (range from 0 to 255):
        analogWrite(LedStrip2, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        //Serial.println(fadeValue);
        delay(10);
      }
    }
    LedStatus2 = false;
//*/
  }
}