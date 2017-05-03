// защелка на 74HC165
int latchPinIN = 7;
// выход последовательных данных на 74HC165
int dataPinIN = 8;
// тактовая ножка
int clockPinIN = 6; 
// переменная считываемых с IN данных
int value_in1 = 0; // переменная первого IN
int value_in2 = 0; // переменная вторгоо IN

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // инициализируем 74HC165 (входной сдвиговый регистр)
  pinMode(latchPinIN, OUTPUT);
  pinMode(dataPinIN, INPUT);
  pinMode(clockPinIN, OUTPUT);
  
  // защелкиваем состояние входных пинов регистра дабы он ничего не ожидал пока.
  digitalWrite(latchPinIN, HIGH); 
  
}

void loop() {


  // put your main code here, to run repeatedly:

          // Обрабатываем данные со входящих регистры
          // ----------------------------------------
          // защелкиваем состояние входных пинов во внутреннюю память регистра
          digitalWrite(clockPinIN, HIGH); 
          digitalWrite(latchPinIN, LOW); 
          digitalWrite(latchPinIN, HIGH); 

          //value_in1 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
          //value_in2 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
          value_in1 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);
          value_in2 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);

          //Serial.println(value_in1);
          //Serial.println(value_in2);
          
          // открываем входной1 регистр. пусть слушает.


Serial.print(bitRead(value_in1, 0));
Serial.print("-");
Serial.print(bitRead(value_in1, 1));
Serial.print("-");
Serial.print(bitRead(value_in1, 2));
Serial.print("-");
Serial.print(bitRead(value_in1, 3));
Serial.print("-");
Serial.print(bitRead(value_in1, 4));
Serial.print("-");
Serial.print(bitRead(value_in1, 5));
Serial.print("-");
Serial.print(bitRead(value_in1, 6));
Serial.print("-");
Serial.print(bitRead(value_in1, 7));
Serial.print("-");
Serial.print(bitRead(value_in2, 0));
Serial.print("-");
Serial.print(bitRead(value_in2, 1));
Serial.print("-");
Serial.print(bitRead(value_in2, 2));
Serial.print("-");
Serial.print(bitRead(value_in2, 3));
Serial.print("-");
Serial.print(bitRead(value_in2, 4));
Serial.print("-");
Serial.print(bitRead(value_in2, 5));
Serial.print("-");
Serial.print(bitRead(value_in2, 6));
Serial.print("-");
Serial.print(bitRead(value_in2, 7));
Serial.println(";");
   delay (100);

//         Serial.println(value_in2);
/*
           int sw1 = bitRead(value_in1, 0);
           int sw2 = bitRead(value_in1, 1);
           int sw3 = bitRead(value_in1, 2);
           int sw4 = bitRead(value_in1, 3);
           int sw5 = bitRead(value_in1, 4);
           int sw6 = bitRead(value_in1, 5);
           int sw7 = bitRead(value_in1, 6);
           int sw8 = bitRead(value_in1, 7);
           //int sw1 = bitRead(value_in1, 0);
*/
}
