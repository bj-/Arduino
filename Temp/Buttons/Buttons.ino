
long timer = 390; // Timer in seconds (Start Value)

unsigned long startTime = 0;  //When count down timer stared
String addZero = ""; // if minutes or sec0nds less than 10 - add 0 before value
bool timerWorking = false;
//int timeToDown = 2;
unsigned long timeToDown = 1;
int timeToDownMinutes = 0;
int timeToDownSeconds = 0;

bool noMusic = false;
bool Debug = false;
bool inMenu = false; // Если есть меню то у кнопок может быть другой режим работвы.


const int buttonsCnt = 5;
int buttonPin[buttonsCnt] = {9, 5, 6, 7, 8}; //Display buttons: Right - Center - Left
bool buttonClick[buttonsCnt] = {false, false, false, false, false};
bool ignoreButton[buttonsCnt] = {false, false, false, false, false};
int buttonTicCount[buttonsCnt] = {0, 0, 0, 0, 0};
#define TIME_BUTTON 3            // время устойчивого состояния кнопки (* +-2 мс) 

void setup() {
    Serial.begin(9600);
  // put your setup code here, to run once:
  // Set Button pins mode
  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
  //  Serial.println(buttonPin[i]);
    pinMode(buttonPin[i], INPUT);
  }

  pinMode(4, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
//pinMode(LedPin, OUTPUT);
}

void ReadButtonState (int i)
{
  /*
  // Read buttons state
  // Left Button
  if ( digitalRead(buttonPin[i]) == true )
  {
    // если кнопка нажата, считаем тики
    buttonTicCount[i]++;   // +1 к счетчику состояния кнопки

    if ( buttonTicCount[i] >= TIME_BUTTON ) 
    {
      // состояние кнопки не мянялось в течение заданного времени - состояние кнопки стало устойчивым
      buttonClick[i] = true;
    }
  }
  else
  {
    buttonTicCount[i] = 0; // сброс счетчика подтверждений состояния кнопки
    buttonClick[i] = false;
    ignoreButton[i] = false; // сброс флага игнорирования нажатия кнопки. дабы небыло другой реакции на тоже самое нажатие.
  }
*/
  Serial.print(digitalRead(buttonPin[i]));
}


void loop() {
  // put your main code here, to run repeatedly:
  int i;
  for (i = 0; i < buttonsCnt; i++ ) {
    ReadButtonState (i);
  }

  Serial.print("   ");

  Serial.print(digitalRead(4));
  Serial.print(digitalRead(5));
  Serial.print(digitalRead(6));
  Serial.print(digitalRead(7));
  Serial.print(digitalRead(8));
  Serial.print(digitalRead(9));
  Serial.print(digitalRead(10));
  Serial.print(digitalRead(11));
  Serial.print(digitalRead(12));
  Serial.print(digitalRead(13));

  Serial.print("   ");

  Serial.println();
}
