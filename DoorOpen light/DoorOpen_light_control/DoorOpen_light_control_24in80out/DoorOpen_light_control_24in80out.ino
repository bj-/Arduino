/*
    Basic Pin setup:
    ------------                                  ---u----
    ARDUINO   13|-> SCLK (pin 25)           OUT1 |1     28| OUT channel 0
              12|                           OUT2 |2     27|-> GND (VPRG)
              11|-> SIN (pin 26)            OUT3 |3     26|-> SIN (pin 11)
              10|-> BLANK (pin 23)          OUT4 |4     25|-> SCLK (pin 13)
               9|-> XLAT (pin 24)             .  |5     24|-> XLAT (pin 9)
               8|                             .  |6     23|-> BLANK (pin 10)
               7|                             .  |7     22|-> GND
               6|                             .  |8     21|-> VCC (+5V)
               5|                             .  |9     20|-> 2K Resistor -> GND
               4|                             .  |10    19|-> +5V (DCPRG)
               3|-> GSCLK (pin 18)            .  |11    18|-> GSCLK (pin 3)
               2|                             .  |12    17|-> SOUT
               1|                             .  |13    16|-> XERR
               0|                           OUT14|14    15| OUT channel 15
    ------------                                  --------

    -  Put the longer leg (anode) of the LEDs in the +5V and the shorter leg
         (cathode) in OUT(0-15).
    -  +5V from Arduino -> TLC pin 21 and 19     (VCC and DCPRG)
    -  GND from Arduino -> TLC pin 22 and 27     (GND and VPRG)
    -  digital 3        -> TLC pin 18            (GSCLK)
    -  digital 9        -> TLC pin 24            (XLAT)
    -  digital 10       -> TLC pin 23            (BLANK)
    -  digital 11       -> TLC pin 26            (SIN)
    -  digital 13       -> TLC pin 25            (SCLK)
    -  The 2K resistor between TLC pin 20 and GND will let ~20mA through each
       LED.  To be precise, it's I = 39.06 / R (in ohms).  This doesn't depend
       on the LED driving voltage.
    - (Optional): put a pull-up resistor (~10k) between +5V and BLANK so that
                  all the LEDs will turn off when the Arduino is reset.

    If you are daisy-chaining more than one TLC, connect the SOUT of the first
    TLC to the SIN of the next.  All the other pins should just be connected
    together:
        BLANK on Arduino -> BLANK of TLC1 -> BLANK of TLC2 -> ...
        XLAT on Arduino  -> XLAT of TLC1  -> XLAT of TLC2  -> ...
    The one exception is that each TLC needs it's own resistor between pin 20
    and GND.

    This library uses the PWM output ability of digital pins 3, 9, 10, and 11.
    Do not use analogWrite(...) on these pins.

    This sketch does the Knight Rider strobe across a line of LEDs.

    Alex Leone <acleone ~AT~ gmail.com>, 2009-02-03 */

#include "src\Tlc5940\Tlc5940.h"
//#include "Tlc5940.h"
#include <SPI.h>
/* exclude Eth
#include <UIPEthernet.h>
*/

// PWM increase decrease step for TLC5940
#define STEP_INCREASE 1
#define STEP_DECREASE 5

#define OPEN_SENSOR_NORMAL_STATE 0 // 0 - nornmal break; 1 - normal connected

int maxFadeMin = 0;
int maxFadeMax = 4095;

int DebugSw = 16;
bool DebugMode = false;

// Input to output mappings
//const int rows = 80;
const int cols = 4;
const int rows = 80; // With Ethernet module - maximum  48 rows in array
//const int cols = 4;
const int defCurrFade = 50;
//                          TLC_Ch | 165_Ch | MaxFade | CurrFade |
int interfacesMapping[rows][cols] = {
                              {0,       1,     500,        50     }, 
                              {1,       1,     500,        50     }, 
                              {2,       1,     500,        50     }, 
                              {3,       1,     500,        50     }, 
                              {4,       1,     500,        50     }, 
                              {5,       1,     500,        50     }, 
                              {6,       1,     500,        50     }, 
                              {7,       1,     500,        50     }, 
                              {8,       1,     500,        50     }, 
                              {9,       1,     500,        50     }, 
                              {10,      1,     500,        50     }, 
                              {11,      1,     500,        50     }, 
                              {12,      1,     500,        50     }, 
                              {13,      1,     500,        50     }, 
                              {14,      1,     500,        50     }, 
                              {15,      1,     500,        50     }, 
                              
                              {16,      2,     4095,        50     }, 
                              {17,      2,     4095,        50     }, 
                              {18,      2,    4095,        50     }, 
                              {19,      2,    4095,        50     }, 
                              {20,      2,    4095,        50     }, 
                              {21,      2,    4095,        50     }, 
                              {22,      2,    4095,        50     }, 
                              {23,      2,    4095,        50     }, 
                              {24,      2,    4095,        50     }, 
                              {25,      2,    4095,        50     }, 
                              {26,      2,    4095,        50     }, 
                              {27,      2,    4095,        50     }, 
                              {28,      2,    4095,        50     }, 
                              {29,      2,    4095,        50     }, 
                              {30,      2,    4095,        50     }, 
                              {31,      2,    4095,        50     }, 

                              {32,      10,    1500,        50     }, 
                              {33,      10,    1500,        50     }, 
                              {34,      10,    1500,        50     }, 
                              {35,      9,    1500,        50     }, 
                              {36,      9,    1500,        50     }, 
                              {37,      9,    1500,        50     }, 
                              {38,      11,    1500,        50     }, 
                              {39,      11,    1500,        50     }, 
                              {40,      11,    1500,        50     }, 
                              {41,      12,    1500,        50     }, 
                              {42,      12,    1500,        50     }, 
                              {43,      12,    1500,        50     }, 

                              {44,      14,    1500,        50     }, 
                              {45,      14,    1500,        50     }, 
                              {46,      14,    1500,        50     }, 
                              {47,      13,    1500,        50     }, 
                              {48,      13,    1500,        50     }, 
                              {49,      13,    1500,        50     }, 
                              {50,      15,    1500,        50     }, 
                              {51,      15,    1500,        50     }, 
                              {52,      15,    1500,        50     }, 
                              {53,      16,    1500,        50     }, 
                              {54,      16,    1500,        50     }, 
                              {55,      16,    1500,        50     }, 

                              {56,      18,    1500,        50     }, 
                              {57,      18,    1500,        50     }, 
                              {58,      18,    1500,        50     }, 
                              {59,      17,    1500,        50     }, 
                              {60,      17,    1500,        50     }, 
                              {61,      17,    1500,        50     }, 
                              {62,      19,    1500,        50     }, 
                              {63,      19,    1500,        50     }, 
                              {64,      19,    1500,        50     }, 
                              {65,      20,    1500,        50     }, 
                              {66,      20,    1500,        50     }, 
                              {67,      20,    1500,        50     }, 

                              {68,      22,    1500,        50     }, 
                              {69,      22,    1500,        50     }, 
                              {70,      22,    1500,        50     }, 
                              {71,      21,    1500,        50     }, 
                              {72,      21,    1500,        50     }, 
                              {73,      21,    1500,        50     }, 
                              {74,      23,    1500,        50     }, 
                              {75,      23,    1500,        50     }, 
                              {76,      23,    1500,        50     }, 
                              {77,      24,    1500,        50     }, 
                              {78,      24,    1500,        50     }, 
                              {79,      24,    1500,        50     }, 

                            };


// PIN configuration

// защелка на 74HC165 Latch
int latchPinIN = 2;
// выход последовательных данных на 74HC165 Data Pin
int dataPinIN = 8;
// тактовая ножка Clock
int clockPinIN = 5; 

// переменная считываемых с IN данных variable for read inputs state from HC165
int value_in1 = 0; // переменная первого IN
int value_in2 = 0; // переменная вторгоо IN
int value_in3 = 0; // переменная третьего IN


/* exclude Eth
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0x8E, 0xEF, 0xFF, 0xEE
};
IPAddress ip(192, 168, 39, 66);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


String readString = String(30); // строка для выборки данных из адресной строки
*/


void setup()
{

  
  // инициализируем 74HC165 (входной сдвиговый регистр)
  pinMode(latchPinIN, OUTPUT);
  pinMode(dataPinIN, INPUT);
  pinMode(clockPinIN, OUTPUT);

  pinMode(DebugSw, INPUT);
  
  // защелкиваем состояние входных пинов регистра дабы он ничего не ожидал пока.
  digitalWrite(latchPinIN, HIGH); 
  

  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
 Tlc.init();

    pinMode(6, OUTPUT);
   analogWrite(6, 1);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  DebugMode = !digitalRead(DebugSw);
  if ( DebugMode )
  {
    Serial.println(F("Started in Debug mode"));
  }
  else 
  {
    Serial.println(F("Started in Production mode"));
  }

/* exclude Eth
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print(F("server is at "));
  Serial.println(Ethernet.localIP());
*/  
/*
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
*/  
}

/* This loop will create a Knight Rider-like effect if you have LEDs plugged
   into all the TLC outputs.  NUM_TLCS is defined in "tlc_config.h" in the
   library folder.  After editing tlc_config.h for your setup, delete the
   Tlc5940.o file to save the changes. */


int setPWMLevel(int sw, int channel, int fadeValue, int maxFade)
{

 
  int ReadSW = sw; // read door switch
  //delay(1);

  if ( ReadSW == OPEN_SENSOR_NORMAL_STATE and fadeValue < ( maxFade - STEP_INCREASE ) )
  {
    //fadeValue++;
    fadeValue = fadeValue + STEP_INCREASE;
    //analogWrite(LedStrip, fadeValue);
  }
  else if  ( ReadSW == OPEN_SENSOR_NORMAL_STATE and fadeValue > ( maxFade - STEP_DECREASE ) )
  {
    if (fadeValue < STEP_DECREASE) // becouse result can jump over zero
    {
      fadeValue = 0;
    }
    else
    {
      fadeValue = fadeValue - STEP_DECREASE;
    }
    //fadeValue--;
    //analogWrite(LedStrip, fadeValue);
  }
  else if ( ReadSW != OPEN_SENSOR_NORMAL_STATE and fadeValue > 0 )
  {
    if (fadeValue < STEP_DECREASE) // becouse result can jump over zero
    {
      fadeValue = 0;
    }
    else
    {
      fadeValue = fadeValue - STEP_DECREASE;
    }
    // fadeValue = fadeValue - STEP_DECREASE;
    //fadeValue--;
    //analogWrite(LedStrip, fadeValue);
  }

  if ( DebugMode )
  {
    Serial.print(F("Set for channnel: "));
    Serial.print(channel);
    Serial.print(F(" fade value: "));
    Serial.println(fadeValue);
  }
  
    Tlc.set(channel, fadeValue);


  return fadeValue;
}

 void latch () {
   digitalWrite(latchPinIN, LOW);
   digitalWrite(latchPinIN, HIGH);
 }

int readInputState (int channel)
{
  // read channel state from HC165. values placed in value_in1, value_in2

  int value = 0;
  
  if (channel >= 1 and  channel <= 8)
  {
    value = bitRead(value_in1, channel-1);
  }
  else if (channel >= 9 and channel <= 16)
  {
    value = bitRead(value_in2, channel-9);
  }
  else if (channel >= 17 and channel <= 24)
  {
    value = bitRead(value_in3, channel-17);
  }
  else
  {
    Serial.print(F("ERROR: [readInputState] Channel is out of range. Try to read channel ["));
    Serial.print(channel);
    Serial.println(F("];"));
  }
/*
  Serial.print("[readInputState] Read Channel [");
  Serial.print(channel);
  Serial.print("] state is [");
  Serial.print(value);
  Serial.println("];");
*/
  return value;
}

void loop()
{
  DebugMode = !digitalRead(DebugSw);
  
  //  Tlc.clear();


  // Обрабатываем данные со входящих регистры
  digitalWrite(clockPinIN, HIGH);
  latch();
//  value_in1 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);
//  value_in2 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);
  
  value_in3 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
  value_in2 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
  value_in1 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);

  if ( DebugMode )
  {
    Serial.print(F("Read from 165: ["));
    Serial.print(value_in3);
    Serial.print("] [");
    Serial.print(value_in2);
    Serial.print("] [");
    Serial.print(value_in1);
    Serial.println("]");
  }
// ----------------------------------------




/*
 *      Input States   to comport
 *
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
*/

// rrrrrrrrrrrrr


  for(int i = 0; i <= rows-1; i++)
  {
    // Set all channels of TLC5940
    // TLC_Ch | 165_Ch | MaxFade | CurrFade |
    // interfacesMapping[32][4]

  //int fr = interfacesMapping[i][1];
  //Serial.print(F("Channel val: "));
  //Serial.println(interfacesMapping[i][1]);
/*
  Serial.print(F("SetPWMLevel: ["));
  Serial.print(readInputState(interfacesMapping[i][1]));
  Serial.print("], [");
  Serial.print(interfacesMapping[i][0]);
  Serial.print("], [");
  Serial.print(interfacesMapping[i][3]);
  Serial.print("], [");
  Serial.print(interfacesMapping[i][2]);
  Serial.println("];");
*/
    if ( i >= 0 && i <= 15 )
    {
      int iState;
      iState = 1;
      if ( !readInputState(1) == 0 || !readInputState(2) == 0 )
      {
        iState = 0;
      }
      interfacesMapping[i][3] = setPWMLevel(iState, interfacesMapping[i][0], interfacesMapping[i][3], interfacesMapping[i][2]);

      //interfacesMapping[i][3] = setPWMLevel(!readInputState(interfacesMapping[i][1]), interfacesMapping[i][0], interfacesMapping[i][3], interfacesMapping[i][2]);
    }
    else if ( i >= 16 && i <= 31 )
    {
      interfacesMapping[i][3] = setPWMLevel(!readInputState(interfacesMapping[i][1]), interfacesMapping[i][0], interfacesMapping[i][3], interfacesMapping[i][2]);
    }
    else
    {
      interfacesMapping[i][3] = setPWMLevel(readInputState(interfacesMapping[i][1]), interfacesMapping[i][0], interfacesMapping[i][3], interfacesMapping[i][2]);
    }
 
  }


    Tlc.update();

    delay(1);


 /* exclude Eth
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println(F("new client"));
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);


        //Переводим символы из запроса HTTP в стринг
        if (readString.length() < 30)
        {  
          //Сохраняем символы в строку  
          readString += c; //заменяем readString.append(c);  
        }   

        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          Serial.println(readString);

          readString = readString.substring(readString.indexOf(" ")+1);
          readString = readString.substring(0, readString.indexOf(" "));
          readString = readString.substring(0, 30); // подрезаем строку на всякий случай. 
          
          Serial.println(readString);


          if(readString.indexOf("s=") >0 && readString.indexOf("v=") >0)//replaces if(readString.contains("L=1"))  
          {
            String channelID = readString.substring(readString.indexOf("s=") + 2, readString.indexOf("&"));
            String newMaxFade =  readString.substring(readString.indexOf("v=")+2);
            
            Serial.print(F("channelID: "));
            Serial.println(channelID);
            Serial.print(F("newMaxFade: "));
            Serial.println(newMaxFade);

            //Serial.print(F("interfacesMapping.length: "));
            //Serial.println(interfacesMapping[][0].length);
            

            if ((channelID.toInt() >= 0) && (channelID.toInt() <= (rows - 1)))
            {
              if ((newMaxFade.toInt() >= maxFadeMin) && (newMaxFade.toInt() <= maxFadeMax))
              {
                int ch_id = channelID.toInt(); 
                interfacesMapping[ch_id][2] = newMaxFade.toInt();    // MaxFade
                Serial.print(F("New MaxFade ["));
                Serial.print(newMaxFade.toInt());
                Serial.print(F("] for channel ["));
                Serial.print(channelID.toInt());
                Serial.println(F("]"));
              }
            }
            
             
            // makeAction(serverName, action);
          }  
          // обнуляем переменную
          readString = "";


          
          // send a standard http response header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          // client.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.println(F("<html><body>"));

          
          
          //client.println(F("<form>"));
          client.println(F("<form action='/' method='get'><button>Refresh</button></form>"));
          client.println(F("<table width='500px'><tr><th>TLC Channel</th><th>Input Sw Ch</th><th>Input Sw State</th><th>Output Fade</th><th>Max Fade</th><th width='150px'>Set Max Fade</th></tr>"));



          for(int i = 0; i <= rows-1; i++)
          {
            // Set all channels of TLC5940
            // TLC_Ch | 165_Ch | MaxFade | CurrFade |
            // interfacesMapping[32][4]
  
            client.print(F("<tr><td>"));
            
            client.print(interfacesMapping[i][0]);  // TLC_Ch
            client.print(F("</td><td>"));
            client.print(interfacesMapping[i][1]);    // 165_Ch
            client.print(F("</td><td>"));
            client.print(readInputState(interfacesMapping[i][1]));    // 165_Ch State
            client.print(F("</td><td>"));
            client.print(interfacesMapping[i][3]);    // CurrFade 
            client.print(F("</td><td>"));
            client.print(interfacesMapping[i][2]);    // MaxFade
            client.print(F("</td><td>"));
            client.print(F("<form action='/' method='get'><input type='hidden' name='s' value='"));
            client.print(interfacesMapping[i][0]);  // TLC_Ch
            client.print(F("' /><input type='text' name='v' value='500' size='4' /><button>Set!</button></form>"));
            client.println(F("</td></tr>"));

            

            // TLC_Ch
            //interfacesMapping[i][0]
            // 165_Ch
            //readInputState(interfacesMapping[i][1])
            // MaxFade
            //interfacesMapping[i][2]
            // CurrFade
            //interfacesMapping[i][3]
  
          }

          
          client.println(F("</table></body></html>"));
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println(F("client disconnected"));

    //String ssdd = "aaa"+"bbb";
   
    //Serial.println(ssdd);
  }
*/

}
