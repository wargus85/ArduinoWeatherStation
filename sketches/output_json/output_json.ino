/*

Some source code for the weather station originally from: https://www.dfrobot.com/wiki/index.php/Weather_Station_with_Anemometer/Wind_vane/Rain_bucket_SKU:SEN0186
Some more source code for the http server from: https://www.arduino.cc/en/Tutorial/WebServer
Time Library: https://www.pjrc.com/teensy/td_libs_Time.html

All other additions/modifications by Warren Argus.

Example data output:
{"coord":{"lon":-32.,"lat":115},"main":{"temp": -2977.78,"pressure":-904.00,"humidity":-528},"wind":{"speed:" -2381.83,"gust":-2381.83,"deg":-5328},"rain":{"1h": -1353.31,"24h":-1353.31},"dt":{"date": "11/6/2018","timestamp":1528686180,"readabletime":"11:03AM"}}

In the current configuration below, the arduino will get a DHCP address. My router will statically assign it the same address.
You may want to change the latitude and longitude to the location of your weather station. 

*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <TimeLib.h>

char databuffer[35];
double temp;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
unsigned long epoch;
time_t curtime;
String Cardinal = "N";
bool NotAccurate = true;

float BarMax;
float BarMin;
float TempMax;
float TempMin;
float HumidMax;
float HumidMin;

EthernetServer server(80);

unsigned int localPort = 8888; 
char timeServer[] = "au.pool.ntp.org";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void sendNTPpacket(char* address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void getBuffer()                                                                    //Get weather status data
{
  int index;
  for (index = 0;index < 35;index ++)
  {
    if(Serial.available())
    {
      databuffer[index] = Serial.read();
      if (databuffer[0] != 'c')
      {
        index = -1;
      }
    }
    else
    {
      index --;
    }
  }
}

int transCharToInt(char *_buffer,int _start,int _stop)                               //char to int）
{
  int _index;
  int result = 0;
  int num = _stop - _start + 1;
  int _temp[num];
  for (_index = _start;_index <= _stop;_index ++)
  {
    _temp[_index - _start] = _buffer[_index] - '0';
    result = 10*result + _temp[_index - _start];
  }
  return result;
}

int WindDirection()                                                                  //Wind Direction
{
  return transCharToInt(databuffer,1,3);
}

float WindSpeedAverage()                                                             //air Speed (1 minute)
{
  temp = 0.44704 * transCharToInt(databuffer,5,7);
  return temp;
}

float WindSpeedMax()                                                                 //Max air speed (5 minutes)
{
  temp = 0.44704 * transCharToInt(databuffer,9,11);
  return temp;
}

float Temperature()                                                                  //Temperature ("C")
{
  temp = (transCharToInt(databuffer,13,15) - 32.00) * 5.00 / 9.00;
  return temp;
}

float RainfallOneHour()                                                              //Rainfall (1 hour)
{
  temp = transCharToInt(databuffer,17,19) * 25.40 * 0.01;
  return temp;
}

float RainfallOneDay()                                                               //Rainfall (24 hours)
{
  temp = transCharToInt(databuffer,21,23) * 25.40 * 0.01;
  return temp;
}

int Humidity()                                                                       //Humidity
{
  return transCharToInt(databuffer,25,26);
}

float BarPressure()                                                                  //Barometric Pressure
{
  temp = transCharToInt(databuffer,28,32);
  return temp / 10.00;
}
String GetCardinal() //Cardinal directions. The sensors only report 8 values.
{
  if (WindDirection() == 0 ){
    Cardinal = "N";
  }
  if (WindDirection() == 45 ) {
    Cardinal = "NE";
  }
  if (WindDirection() == 90 ) {
    Cardinal = "E";
  }
  if (WindDirection() == 135 ) {
    Cardinal = "SE";
  }
  if (WindDirection() == 180 ) {
    Cardinal = "S";
  }
  if (WindDirection() == 225 ) {
    Cardinal = "SW";
  }
  if (WindDirection() == 270 ) {
    Cardinal = "W";
  }
  if (WindDirection() == 315 ) {
    Cardinal = "NW";
  }
  return Cardinal;
}

void setup(){
  // start the Ethernet connection and the server:
  Ethernet.begin(mac) == 0;
  server.begin();
  Udp.begin(localPort);
  Serial.begin(9600);
  sendNTPpacket(timeServer);
  delay(1000);
  if (Udp.parsePacket()) {
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    epoch = secsSince1900 - seventyYears;
    delay(1000);
    setTime(epoch);
  }

}

void loop(){
// listen for incoming clients
EthernetClient client = server.available();
if (client) {
    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          //Tell the client this is a JSON
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json; charset=utf-8");
          client.println("Connection: keep-alive");
          client.println();

          curtime = now(); //get the current time
          
          do {
            //get the data and test it is range
            getBuffer();
            while (BarPressure() >= 1041 || BarPressure() <=  979 || Temperature() >= 55 || Temperature() <= -5 || Humidity() > 100 || Humidity() < 0 ) {
            //data is not in range
            delay(250);
            getBuffer();
            }
            //Now the data is in range, we need to save the data values and compare it. Only once.
            //If it's bad, then try it all again.
            BarMax = (BarPressure() +5);
            BarMin = (BarPressure() -5);
            TempMax = (Temperature() +5);
            TempMin = (Temperature() -5);
            HumidMax = (Humidity() +5);
            HumidMin = (Humidity() -5);
            
            getBuffer();

            while (BarPressure() >= 1041 || BarPressure() <=  979 || Temperature() >= 55 || Temperature() <= -5 || Humidity() > 100 || Humidity() < 0 ) {
            //data is not in range
            delay(250);
            getBuffer();
            }
            
            if (BarPressure() > BarMax || BarPressure() < BarMin || Temperature() > TempMax || Temperature() < TempMin || Humidity() > HumidMax || Humidity() < HumidMin ) {
              //our data is bad
              NotAccurate = true;
            }
            else {
              //our data is good
              NotAccurate = false;
            }

          }while (NotAccurate);
          
          //Create a json formatted string
          String data = "{\"coordlocal\":{\"lon\":115.86,\"lat\":-32.00},\"weather\":{\"temp\":"
          +String(Temperature())+",\"pressure\":"
          +String(BarPressure())+",\"humidity\":"
          +String(Humidity())+"},\"wind\":{\"localspeed\":"
          +String(WindSpeedAverage())+",\"localgust\":"
          +String(WindSpeedMax())+",\"localdeg\":"
          +String(WindDirection())+",\"cardinal\":\""
          +GetCardinal()+"\"},\"localrain\":{\"1h\":"
          +String(RainfallOneHour())+",\"24h\":"
          +String(RainfallOneDay())+"},\"localdt\":"
          +String(curtime)+"}";
          
          //send the data to the webclient
          client.println(data);
          
          //set the data to dirty
          NotAccurate = true;

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
    delay(250);
    // close the connection:
    client.stop();
  }
  Ethernet.maintain(); //renew the dhcp lease if necessary
}