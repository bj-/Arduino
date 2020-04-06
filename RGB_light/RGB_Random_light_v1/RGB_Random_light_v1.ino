const int RED = 11;
const int GREEN = 10;
const int BLUE = 9;

int sRed = 10;
int sGreen = 5;
int sBlue = 50;

int cRed = 255;
int cGreen = 255;
int cBlue = 255;

int tRed = 255; // к чему стремится канал
int tGreen = 255;
int tBlue = 255;

int timer = 0;
int cMills = 0;
int pMills = 0; //пред. значение

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  analogWrite(RED, 255);
  analogWrite(GREEN, 255);
  analogWrite(BLUE, 255);

  Serial.begin(9600);
}

int calcNewFade(int S,int C,int T){
  if ( cMills > (pMills + S) )
  {
//  Serial.print("AA");
    if ( C != T )
    {
//  Serial.print("BB");
      if ( C < T )
      {
//  Serial.print("CC");
        C = C + 1;
      }
      else
      {
  //Serial.print("EE");
        C = C -1;
      }
    }
    else 
    {
      T = 255-T;
    }
  }  
  return C;
}
void loop(){  

  cMills = millis();

// RED
cRed = calcNewFade(sRed,cRed,tRed);
if ( cRed == tRed ) { tRed = 255 - tRed; };
  

//GREEN
cGreen = calcNewFade(sGreen,cGreen,tGreen);
if ( cGreen == tGreen ) { tGreen = 255 - tGreen; };
/*
  if ( cMills > (pMills + sGreen) )
  {
  //Serial.print("AA");
    if ( cGreen != tGreen )
    {
//  Serial.print("BB");
      if ( cGreen < tGreen )
      {
//  Serial.print("CC");
        cGreen = cGreen + 1;
      }
      else
      {
  //Serial.print("EE");
        cGreen = cGreen -1;
      }
    }
    else 
    {
      tGreen = 255-tGreen;
    }
  }  
*/
//Blue
cBlue = calcNewFade(sBlue,cBlue,tBlue);
if ( cBlue == tBlue ) { tBlue = 255 - tBlue; };
/*
Serial.print("Mills: [");
Serial.print(cMills);
Serial.print("] pMills: [");
Serial.print(pMills);
Serial.print("] sRed: [");
Serial.print(sRed);
Serial.print("] tRed: [");
Serial.print(tRed);
Serial.print("] cRed [");
Serial.print(cRed);
Serial.print("] cGreen [");
Serial.print(cGreen);
Serial.print("] cBlue [");
Serial.print(cBlue);
Serial.print("] tBlue [");
Serial.println(tBlue);
*/
    analogWrite(RED, cRed);
    analogWrite(GREEN, cGreen);
    analogWrite(BLUE, cBlue);
    //analogWrite(RED, 255);
    //analogWrite(GREEN, 0);
    //analogWrite(BLUE, 255);

pMills = cMills;

//  int NewValRed = random(0, 255); //сгенерим новые значения
//  int NewValGreen = random(0, 255);
//  int NewValBlue = random(0, 255);

  
  //for(int i=0;i<=255;i++){
//    analogWrite(RED, 255-i);
//  }
  

//  for(int i=0;i<=255;i++){
    //analogWrite(RED, 255-i);
    //delay(10);
  //}

  delay(10);

  // Вызываем функцию установки цвета и в качестве параметров передаем номер пина. 
  // Используем вызов 3 раза со сменой пинов, для перетекания из одного цвета в другой.
  ///setFadeColor(RED,GREEN,BLUE);
  //setFadeColor(GREEN,BLUE,RED);
  //setFadeColor(BLUE,RED,GREEN);
  
}
 
// Функция, которая устанавливает для каждого пина(RGB канала) 
// свой алгоритм изменения цвета
void setFadeColor(int cPin1,int cPin2,int cPin3){
//  for(int i=0;i<=255;i++){
  for(int i=255;i>=0;i--){
    analogWrite(cPin1, i);
    analogWrite(cPin2, 255-i);
    analogWrite(cPin3, 255);
    delay(5);
  }
}
