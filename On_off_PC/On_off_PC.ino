#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xD1, 0xAD, 0xBA, 0x7F, 0x9A, 0xEF
};
IPAddress ip(192, 168, 39, 177);

EthernetServer server(80);


// Пин подключен к MR (Сброс) - Pin 10 у 74HC595 (сброс значений регистра. по LOW)
int mrPin = 2;
//Пин подключен к SH_CP (Тактовая) - Pin 11 у 74HC595 
int clockPin = 3; 
//Пин подключен к ST_CP (Защелка) - Pin 12 у 74HC595 
int latchPin = 4; 
// Пин подключен к OE (состоние он/офф) - Pin 13 у 74HC595 (переход в рабочее состояние по LOW)
//int oePin = 5;
//Пин подключен к DS (данные вход на 595) - Pin 14 у 74HC595 
int dataPin = 5; 
// Пин подключен к транзистору включающему цепь оптопар. HIGH - включено
int optoOn = 6;


// защелка на 74HC165
int latchPinIN = 8;
// выход последовательных данных на 74HC165
int dataPinIN = 9;
// тактовая ножка
int clockPinIN = 7; 
// переменная считываемых с IN данных
int value_in1 = 0; // переменная первого IN
int value_in2 = 0; // переменная вторгоо IN


//boolean incoming = 0;
String readString = String(30); // строка для выборки данных из адресной


// Массив серверов
//   0 - Название
//   1 - Кнопка Power (номер пина на выходном сдвиговом регистре)
//   2 - Кнопка Reset
//   3 - контроль питания (номер пина на входном сдвиговом регистре))
//   максимум 12 т.к. больше нет выходов у регистров на данный момент.

String serverList[12][4] = {{"Dfecs1Main", "0",  "1", "1"},
                            {"FI-HRV1",    "2",  "3", "2"},
                            {"FI-HRV2",    "4",  "5", "3"}, 
                            {"FI-HRV3",    "6",  "7", "4"},
                            {"FI-RRAS",    "8",  "9", "5"},
                            {"FI-SRV1",   "10", "11", "6"},
                            {"FI-NAS",    "12", "13", "7"},
                            {"Render1",   "14", "15", "9"},
                            {"Render2",   "16", "17", "10"},
                            {"Render3",   "18", "19", "11"},
                            {"TestPC",    "20", "21", "12"},
                            {"EMPTY",     "22", "23", "13"}};

/*
String serverList[] = {"Dfecs1Main:0:1:0",
                       "FI-HRV1:2:3:1",
                       "FI-HRV2:4:5:2", 
                       "FI-HRV3:6:7:3",
                       "FI-RRAS:8:9:4",
                     "FI-SRV1:10:11:5",
                      "FI-NAS:12:13:6",
                     "Render1:14:15:7",
                     "Render2:16:17:8",
                     "Render3:18:19:9",
                      "TestPC:20:21:10",
                       "EMPTY:22:23:11"};

*/
//int inputFISRV1 = 5;


// Выполняем действие с сервером
void makeAction(String serverName, String action)
{
  // если входяцие переменныне не нулевой длины то будем что-то делать
  if (serverName.length() and action.length())
  {
    
    int tempInt;
    int p = 0;
    
    // смотрим что надо делать и устанавливаем соответвующий пин из массива серверов)
    if (action == "p") p = 1;
    else if (action == "l") p = 1;
    else if (action == "r") p = 2;
    
    // перебираем массив серверов в поиске нужного
    for (int i = 0; serverList[i][0].length() > 0; i++ )
    {
      if (serverList[i][0] == serverName) tempInt = serverList[i][p].toInt();
    }

    if (p) // проверяем имеет ли смыл что-то делать
    {
      // активируем цепь
      registerWrite(tempInt, HIGH); 
      digitalWrite(optoOn, HIGH); // включаем цепь оптопар
    
      if (action == "l") delay(8000); // Длинное нажатие на питание
      else delay(500); // на ресет и на короткое нажатие павера
    
      // деактевируем.
      digitalWrite(optoOn, LOW);  // выключаем цепь оптопар
      registerWrite(tempInt, LOW);
    }

  }
}


// =========
//   SETUP
// =========
void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
//  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  

  // пины мониторинга включенности серверов
//  pinMode(inputFISRV1, INPUT);


//  pinMode(testLed2, OUTPUT);

/*  
  pinMode(testLed1, OUTPUT);
  pinMode(testLed3, OUTPUT);

  // выключаем тестовый светодиод
  digitalWrite(testLed1, LOW);
  digitalWrite(testLed3, LOW);
*/
  //устанавливаем режим OUTPUT Для сдвивого регистра
  pinMode(latchPin, OUTPUT); 
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, OUTPUT); 
  pinMode(optoOn, OUTPUT); 
  pinMode(mrPin, OUTPUT); 
//  delay(4000);  // -------------------------------   УБРАТЬ
//  digitalWrite(oePin, HIGH);  // ВЫКЛючаем мультиплексор
  digitalWrite(mrPin, LOW);  // сброс значений регистра
delay(50);
  digitalWrite(mrPin, HIGH); // переводим в рабочее состояние после сброса
//  digitalWrite(oePin, LOW); // ВКЛючаем мультиплексор

//  makeAction("EMPTY", "p");
  registerWrite(23, LOW); // еще один сброс. методом записи нулей во все биты

  // инициализируем 74HC165 (входной сдвиговый регистр)
  pinMode(latchPinIN, OUTPUT);
  pinMode(dataPinIN, INPUT);
  pinMode(clockPinIN, OUTPUT);
  
  // защелкиваем состояние входных пинов регистра дабы он ничего не ожидал пока.
  digitalWrite(latchPinIN, HIGH); 

//  digitalWrite(5, LOW);  // ВКЛючаем 595


}


// Этот метот записывает байт в регистр 
void registerWrite(int whichPin, int whichState) { 

  // Сбрасываем значения регистров. ибо старые нам не нужны.
  digitalWrite(mrPin, LOW);  // сброс значений регистра
  digitalWrite(mrPin, HIGH); // переводим в рабочее состояние

  
  // инициализируем и обнуляем байт 
  byte bitsToSend[3] = {0, 0, 0}; 

  // зачелка в лоу - регистр готов прнимать данные
  digitalWrite(latchPin, LOW); 

  // устанавливаем HIGH в соответствующем бите 
  bitWrite(bitsToSend[whichPin/8], whichPin%8, whichState);  

  // проталкиваем байт в регистр 
  // MSBFIRST - обхот осуществляется по порядку = порядку ног, но регистры обходятся в обратном порядке
  // LSBFIRST - обхот осуществляется в обратном порядке и ноги и регистры
  //  for(int i = 0; i < 3; i++) shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend[i]);  
//  for(int i = 0; i < 3; i++) shiftOut(dataPin, clockPin, LSBFIRST, bitsToSend[i]);  
  for(int i = 0; i < 3; i++) shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend[i]);  

    // "защелкиваем" регистр, чтобы байт появился на его выходах 
  digitalWrite(latchPin, HIGH);  
  
} 



void loop() {



//delay(1000);

/*
  if (value_in1 != 255) 
  {
    registerWrite(10, HIGH);
    delay(100);
    registerWrite(11, HIGH);
    delay(100);
    registerWrite(12, HIGH);
    delay(3000);
  }
/*
  if (value_in2 != 255) 
  {
    registerWrite(20, HIGH);
    delay(100);
    registerWrite(21, HIGH);
    delay(100);
    registerWrite(22, HIGH);
    delay(3000);
  }
 */
//  Serial.println(value_in1);
//  Serial.println(value_in2);
  
  
/*  
  // проходим циклом по всем 24 выходам трех регистров 
  digitalWrite(optoOn, HIGH);

  for (int i = 0; i < 24; i++) { 
    // записываем сигнал в регистр для очередного светодиода 
    registerWrite(i, HIGH); 
    // делаем паузу перед следующией итерацией 
    delay(10); 
  } 

  digitalWrite(optoOn, LOW);
*/

  digitalWrite(mrPin, LOW);  // сброс значений регистра
  digitalWrite(mrPin, HIGH); // переводим в рабочее состояние
  


  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    // Метка что можно анчинать читать параметыр из запроса (взводится в тру после символа "?"
    //boolean getString = false;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

//        Serial.print(c);

        //Переводим символы из запроса HTTP в стринг
        if (readString.length() < 30)
        {  
          //Сохраняем символы в строку  
          readString += c; //заменяем readString.append(c);  
        }   

//        Serial.print(readString);
        
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

//          Serial.println("AAAAAA");
//          Serial.println(readString);
          
          readString = readString.substring(readString.indexOf(" ")+1);
          readString = readString.substring(0, readString.indexOf(" "));
          readString = readString.substring(0, 30); // подрезаем строку на всякий случай. 
          
//          Serial.println(readString);
          //delay(1000);
          

          if(readString.indexOf("s=") >0 && readString.indexOf("a=") >0)//replaces if(readString.contains("L=1"))  
          {
            String serverName = readString.substring(readString.indexOf("s=") + 2, readString.indexOf("&"));
            String action =  readString.substring(readString.indexOf("a=")+2);
             
            makeAction(serverName, action);
               
            //Serial.println("ServerName is [" + serverName + "] Action is [" +  action + "]");
            //digitalWrite(ledPinR, HIGH);    // set the LED on  
          }  
          // обнуляем переменную
          readString = "";

          // Обрабатываем данные со входящих регистры
          // ----------------------------------------
          // защелкиваем состояние входных пинов во внутреннюю память регистра
          digitalWrite(latchPinIN, HIGH); 

          //value_in1 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
          //value_in2 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
          value_in1 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);
          value_in2 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);

          //Serial.println(value_in1);
          //Serial.println(value_in2);
          
          // открываем входной1 регистр. пусть слушает.
          digitalWrite(latchPinIN, LOW); 

/*
          for (int i = 0; i < 8; i++)
          {
            if (bitRead(value_in1, i)) 
            {
              registerWrite(i, HIGH); 
//              Serial.println(i);
      
            }
            if (bitRead(value_in2, i)) 
            {
              registerWrite(i+8, HIGH); 
              Serial.println(i);
            }      
          }
*/          
          
          String td = "<td>";
          String ctd = "</td>";
          String option = "option";
          String optionValue = option + " value='";
          int srvState;
          
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html><h1>Server State Monitor</h1><table width=400 align=center border=1 cellspacing=0>");
//                          "<h1>FI Network</h1><p>Production servers control system</p>"
//                          "<table border=1, cellpadding=0, cellspacing=0 width=90%>"
//                          "<tr><th>ServerName</th><th>State</th><th>Make Action</th></tr>");

          // output the value of each analog input pin
          
          // Список серверов
          for (int i = 0; serverList[i][0].length() > 0; i++ )
          {
            client.print("<tr>");
            client.print(td);
//            client.print("<tr><td>");
            client.print(serverList[i][0]);
            client.print(ctd + td);
            
//            Serial.println(value_in1);
//            Serial.println(value_in2);
            
            // Стейт сервера
            if (serverList[i][3].toInt() <= 8 )
            {
              srvState = bitRead(value_in1, serverList[i][3].toInt());
            }
            else
            {
              srvState = bitRead(value_in2, serverList[i][3].toInt()-8);
            }
            if (srvState)
            {
              client.print("Online");
            }
            else
            {
              client.print("off");
            }
            
            client.print(ctd + td);
//            client.print("</td><td>uncknown</td><td>");
            client.print("<form action='/' method='get'><input type='hidden' name='s' value='");
            client.print(serverList[i][0]);

//            client.println("' /><select name='a'><option>None</option><option value='p'>Power</option><option value='l'>LongPower</option>"
//                          "<option value='r'>Reset</option></select><button>GO!</button></form></td></tr>");
            // Вместо строчки сверху - колхозим такого монстра. проигрываем в обземе кода ~150 байт, но выигрываем в обземе памяти ~67 байт
            client.print("' /><select name='a'><");
            client.print(option);
            client.print(">None</");
            client.print(option);
            client.print("><");
            client.print(optionValue);
            client.print("p'>Power</");
            client.print(option);
            client.print("><");
            client.print(optionValue);
            client.print("l'>LongPower</");
            client.print(option);
            client.print("><");
            client.print(optionValue);
            client.print("r'>Reset</");
            client.print(option);
            client.print("></select><button>GO!</button></form>");
            client.print(ctd);
            client.println("</tr>");
          }

          client.println("</table></html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
