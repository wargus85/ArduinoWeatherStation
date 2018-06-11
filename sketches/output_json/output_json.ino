/*
Source code for the weather station originally from: https://www.dfrobot.com/wiki/index.php/Weather_Station_with_Anemometer/Wind_vane/Rain_bucket_SKU:SEN0186
Source code for the http server from: https://www.arduino.cc/en/Tutorial/WebServer
Time Library: https://www.pjrc.com/teensy/td_libs_Time.html

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
//IPAddress ip(10, 60, 204, 199);
EthernetServer server(80);

unsigned int localPort = 8888;       // local port to listen for UDP packets
char timeServer[] = "au.pool.ntp.org"; // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
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

void setup(){
  // start the Ethernet connection and the server:
  Ethernet.begin(mac) == 0;
  server.begin();
  Udp.begin(localPort);
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
    delay(10000);
  //Ethernet.maintain();
  setTime(epoch);
}

}

void loop(){

//pull the data from the serial port
//getBuffer();
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
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 20");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            // output the values of the sensors
            client.print("{\"coord\":{\"lon\":-32.000757,\"lat\":115.869983},");
            client.println("\"main\":{\"temp\":");
            client.print(Temperature());
            client.print(",\"pressure\":");
            client.print(BarPressure());
            client.print(",\"humidity\":");
            client.print(Humidity());
            client.print("},");
            client.println("\"wind\":{\"speed:\"");
            client.print(WindSpeedAverage());
            client.print(",\"gust\":");
            client.print(WindSpeedMax());
            client.print(",\"deg\":");
            client.print(WindDirection());
            client.print("},");
            client.println("\"rain\":{\"1h\":");
            client.print(RainfallOneHour());
            client.print(",\"24h\":");
            client.print(RainfallOneDay());
            client.print("},");
            curtime = now();
            client.println("\"dt\":{\"date\":");
            client.print("\"");
            client.print(day(curtime));
            client.print("/");
            client.print(month(curtime));
            client.print("/");
            client.print(year(curtime));
            client.print("\"");
            client.print(",\"timestamp\":");
            client.print(curtime);
            client.print(",\"readabletime\":");
            client.print("\"");
            client.print(hour()+8);
            client.print(":");
            if (minute() < 10){
              client.print("0");
              client.print(minute());
              }
            else {
              client.print(minute());
              }
            if (hour() < 12) {
              client.print("AM");
            }
            else {
              client.print("PM");
            }
            client.print("\"}}");
            client.println("</html>");
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
  }
}
