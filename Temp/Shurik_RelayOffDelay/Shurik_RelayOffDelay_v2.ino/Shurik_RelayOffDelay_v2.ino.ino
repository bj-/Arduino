int power = 8; // PB0 (CPU 14 Pin) Power led. always on
int relay = 10; // PB2 (CPU 16 Pin)  Relay
int button = 9;  // PB1 (CPU 15 Pin) Button
int adjustment = A3;  // ADC3 (CPU Pin 26) ( 10k adj resistor (to adjust time delay)

int delayIncrease = 2; // PD2 (CPU Pin 4 )
int delayDecrease = 3; // PD3 (CPU Pin 5 )



int sensorValue = 0;        // value read from the Adjustment Resistor
int outputValue = 0;        // mapped value for Led panel
int oldOutputValue = 0;     // old value. ifvalue changed - to update LED display
int delayValue = 0;         // if CPU = 8MHz = value / 2
int numberToShow = 0;       // Show value to LED Display

void setup()
{
  Serial.begin(9600);
  
  
  pinMode(power, OUTPUT);
  digitalWrite(power, HIGH);

  pinMode(relay, OUTPUT);

  pinMode(button, INPUT);

  pinMode(adjustment, INPUT);
  
}


void loop()
{
  
    // read the analog in value:
    sensorValue = analogRead(adjustment);
    // map it to the range of the analog out:
    outputValue = map(sensorValue, 0, 1023, 0, 250);
    
    delayValue = (int) outputValue * 10 / 2;
  

//    if (oldOutputValue != outputValue)
//    if ( (oldOutputValue+1 < outputValue) or  (oldOutputValue-1 > outputValue) )
//    if ( oldOutputValue != outputValue )
    if ( (oldOutputValue+1 < outputValue) or  (oldOutputValue > outputValue) )
    {
      Serial.print(outputValue);
      
      oldOutputValue = outputValue;
      delay(20);
    }

  
  if (digitalRead(button))
  {
    digitalWrite(relay, HIGH);
//    int i = map()


    // print the results to the serial monitor:
//    Serial.print("sensor = " );
//    Serial.print(sensorValue);
//    Serial.print("\t output = ");
//    Serial.println(outputValue);



    
    delay(delayValue);

    while (digitalRead(button))
    {
      digitalWrite(relay, LOW);
      delay(100);
    }

    
  }
  else
  {
    digitalWrite(relay, LOW);
  }
}
