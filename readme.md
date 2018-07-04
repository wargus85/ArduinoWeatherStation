# Arduino/Elasticsearch Weather Station Project

This project is inspired by and hopefully extends [This project](https://elastic.co/blog/arduino-based-home-weather-station-on-the-elastic-stack#sketch1) 

The premise is simple enough: A homebrew weather station that exports a JSON of data into elasticsearch. I've extended it further in two ways:
1. Firstly, by also importing data from a Fronius inverter via logstash into Elasticsearch. Fronius Inverters have a API that exports data in a JSON format. This was a crime of convenience. 
2. Secondly, by adding a script, written in Python 3 that will publish my data to the UK Met Office [Weather Observation Website](http://wow.metoffice.gov.uk/). 

Detailed Instructions can be found on [my blog](https://blog.warbel.net/index.php/2018/06/29/building-an-arduino-weather-station-with-elk-stack/)

## Hardware required for the project:
1. [Weather Station Kit with Anemometer/Wind Vane/Rain Bucket](https://www.dfrobot.com/wiki/index.php/Weather_Station_with_Anemometer/Wind_vane/Rain_bucket_SKU:SEN0186)
3. [Arduino EtherTen](https://www.freetronics.com.au/products/etherten) (or any arduino with a compatible ethernet shield)
4. Arduino power supply and [Passive PoE Injector](https://www.littlebirdelectronics.com.au/passive-poe-injector-cable-set) Cable Set
5. Cat6 Cabling, project box, misc cables, screws, electrical tape, and weather station roof mounting supplies from your local hardware store.

## Code included in the project/Instructions.

I assume you already have an ELK stack on a Linux box somewhere, and that your arduino is connect to it via the network. Having said that, you don't necessarily need ELK. If you know what you're doing with python, you could just as easily read the JSON and do what you will or simply modify and use my script to upload your data to the MET Office. I like the ELK stack because (after a steep learning curve) it's easy to create my own graphs of the data.

The conf.d directory contains the logstash configuration pipelines you'll need in order to pull the data in from the solar inverter and arduino weather station. The pipelines.yml file contains the pipelines for logstash.

The Sketches directory contains the sketch you'll need to push to your arduino. You may need to download the latest arduino IDE and the time library which is necessary to get the correct time via the network.

The upload_data.py script has its own instructions, but it will pull the data from the arduino and upload it to the Met WOW project. It needs to be hosted (typically) on a Linux machine. Crontab will need to run the script every 5 minutes. Strictly speaking the Arduino could do all of this itself too...

Finally the mappings_script contains information on how to map the weather datatypes into Elasticsearch.