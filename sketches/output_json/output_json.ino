/*
Source code for the weather station originally from: https://www.dfrobot.com/wiki/index.php/Weather_Station_with_Anemometer/Wind_vane/Rain_bucket_SKU:SEN0186
Source code for the http server from: https://www.arduino.cc/en/Tutorial/WebServer

*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10, 60, 204, 199);
EthernetServer server(80);


char databuffer[35];
double temp;

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
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}
void loop(){

//pull the data from the serial port
//getBuffer();
// listen for incoming clients
EthernetClient client = server.available();
if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            // output the values of the sensors
            client.print("{'coord':{'lon':-32.000757,'lat':115.869983},");
            client.println("'main':{'temp':");
            client.print(Temperature());
            client.print(",'pressure':");
            client.print(BarPressure());
            client.print(",'humidity':");
            client.print(Humidity());
            client.print("},");
            client.println("'wind':{'speed:'");
            client.print(WindSpeedAverage());
            client.print(",'gust':");
            client.print(WindSpeedMax());
            client.print(",'deg':");
            client.print(WindDirection());
            client.print("},");
            client.println("'rain':{'1h':");
            client.print(RainfallOneHour());
            client.print(",'24h':");
            client.print(RainfallOneDay());
            client.print("}");
            client.println("}");
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
    Serial.println("client disconnected");
  }
}
