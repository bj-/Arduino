
int ledPin = 6;
#define analogInPin  A3

void setup() {
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue = analogRead(analogInPin);
  
  int fadeValue = map(sensorValue, 0, 1023, 0, 255);

  analogWrite(ledPin, fadeValue);
  
  Serial.print(fadeValue);
  Serial.print(" -- ");
  Serial.println(sensorValue);
  delay(1);        // delay in between reads for stability
}
