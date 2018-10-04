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

#include "Tlc5940.h"
#include <SPI.h>
#include <UIPEthernet.h>
/*
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xD1, 0xAD, 0xBA, 0x7F, 0x9A, 0xEF
};
IPAddress ip(192, 168, 39, 177);

EthernetServer server(80);
*/

// PWM increase decrease step for TLC5940
#define STEP_INCREASE 1
#define STEP_DECREASE 5

#define OPEN_SENSOR_NORMAL_STATE 0 // 0 - nornmal break; 1 - normal connected

int maxFadeMin = 0;
int maxFadeMax = 4095;


// Input to output mappings
const int rows = 32;
const int cols = 4;
//                          TLC_Ch | 165_Ch | MaxFade | CurrFade |
int interfacesMapping[rows][cols] = {
                              {0,       11,     1500,        50     }, 
                              {1,       11,     1500,        50     }, 
                              {2,       11,     1500,        50     }, 
                              {3,       11,     1500,        50     }, 
                              {4,       11,     1500,        50     }, 
                              {5,       11,     1500,        50     }, 
                              {6,       11,     1500,        50     }, 
                              {7,       11,     1500,        50     }, 
                              {8,       10,     1500,        50     }, 
                              {9,       10,     1500,        50     }, 
                              {10,      12,     1500,        50     }, 
                              {11,      13,     1500,        50     }, 
                              {12,      11,     1500,        50     }, 
                              {13,      11,     1500,        50     }, 
                              {14,      10,     1500,        50     }, 
                              {15,      11,     1500,        50     }, 
                              {16,      13,     1500,        50     }, 
                              {17,      12,     1500,        50     }, 
                              {18,      10,    1500,        50     }, 
                              {19,      10,    1500,        50     }, 
                              {20,      11,    1500,        50     }, 
                              {21,      11,    1500,        50     }, 
                              {22,      12,    1500,        50     }, 
                              {23,      12,    1500,        50     }, 
                              {24,      13,    1500,        50     }, 
                              {25,      13,    1500,        50     }, 
                              {26,      13,    1500,        50     }, 
                              {27,      13,    1500,        50     }, 
                              {28,      13,    1500,        50     }, 
                              {29,      13,    1500,        50     }, 
                              {30,      13,    1500,        50     }, 
                              {31,      13,    1500,        50     }, 
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


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0x8E, 0xEF, 0xFF, 0xED
};
IPAddress ip(192, 168, 39, 65);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


String readString = String(30); // строка для выборки данных из адресной строки



void setup()
{

  
  // инициализируем 74HC165 (входной сдвиговый регистр)
  pinMode(latchPinIN, OUTPUT);
  pinMode(dataPinIN, INPUT);
  pinMode(clockPinIN, OUTPUT);
  
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



  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print(F("server is at "));
  Serial.println(Ethernet.localIP());
  
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
  
  //  Tlc.clear();


  // Обрабатываем данные со входящих регистры
  digitalWrite(clockPinIN, HIGH);
  latch();
//  value_in1 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);
//  value_in2 = shiftIn(dataPinIN, clockPinIN, MSBFIRST);
  
  value_in2 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
  value_in1 = shiftIn(dataPinIN, clockPinIN, LSBFIRST);
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


  for(int i = 0; i <= 31; i++)
  {
    // Set all channels of TLC5940
    // TLC_Ch | 165_Ch | MaxFade | CurrFade |
    // interfacesMapping[32][4]

    interfacesMapping[i][3] = setPWMLevel(readInputState(interfacesMapping[i][1]), interfacesMapping[i][0], interfacesMapping[i][3], interfacesMapping[i][2]);
  
  }


    Tlc.update();

    delay(1);


 
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



          for(int i = 0; i <= 31; i++)
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

            
/*
            // TLC_Ch
            interfacesMapping[i][0]
            // 165_Ch
            readInputState(interfacesMapping[i][1])
            // MaxFade
            interfacesMapping[i][2]
            // CurrFade
            interfacesMapping[i][3]
*/  
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
  
}

