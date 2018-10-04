/*
  BMW Moto dMirror
*/

int runLightPin = 5;
int turnPin = 6;
int alertRedPin = 9;
int alertYellowPin = 10;

int reservedPin = 13;

int inputPin = 4;

int heaterPin = 11;


void setup() {
  pinMode(runLightPin, OUTPUT);
  pinMode(turnPin, OUTPUT);
  pinMode(alertRedPin, OUTPUT);
  pinMode(alertYellowPin, OUTPUT);

  pinMode(reservedPin, OUTPUT);

  pinMode(inputPin, INPUT);

  pinMode(heaterPin, OUTPUT);
}

void loop() {

  digitalWrite(runLightPin, HIGH);
  digitalWrite(turnPin, HIGH);
  digitalWrite(alertRedPin, HIGH);
  digitalWrite(alertYellowPin, HIGH);
  digitalWrite(heaterPin, HIGH);
  digitalWrite(reservedPin, HIGH);
  delay(10000);

  // fade out from max to min in increments of 5 points:
  digitalWrite(runLightPin, LOW);
  digitalWrite(turnPin, LOW);
  digitalWrite(alertRedPin, LOW);
  digitalWrite(alertYellowPin, LOW);
  digitalWrite(heaterPin, LOW);
  digitalWrite(reservedPin, LOW);
  delay(1000);
}
