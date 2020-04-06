
int rPin = 3;
int bPin = 9;
int gPin = 10;

int button = 6;
bool buttonState = false;

int DebugSw = 12;
bool DebugMode = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT);

  pinMode(rPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(gPin, OUTPUT);

  digitalWrite(rPin, HIGH);
  digitalWrite(bPin, HIGH);
  digitalWrite(gPin, HIGH);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(button);
  digitalWrite(rPin, buttonState);
}
