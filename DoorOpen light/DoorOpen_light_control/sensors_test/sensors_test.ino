
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
   ReadVal1 = analogRead(OptoSw1);


  int ReadVal2 = 0;


    ReadVal2 = analogRead(OptoSw2);
    delay(1);



  Serial.print("Hall: ");
  Serial.print(ReadVal1);
  Serial.print(", opto: ");
  Serial.println(ReadVal2);
  delay(1);


}
