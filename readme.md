# Arduino/Elasticsearch Weather Station Project

This project is inspired by and hopefully extends [This project](https://elastic.co/blog/arduino-based-home-weather-station-on-the-elastic-stack#sketch1) 

The premise is simple enough: A homebrew weather station that exports a JSON of data into elasticsearch. I've extended it further by also importing data from a Fronius inverter, that also exports its data via a JSON API.

## Hardware required for the project:
1. [Weather Station Kit with Anemometer/Wind Vane/Rain Bucket](https://www.dfrobot.com/wiki/index.php/Weather_Station_with_Anemometer/Wind_vane/Rain_bucket_SKU:SEN0186)
3. [Arduino EtherTen](https://www.freetronics.com.au/products/etherten) (or any arduino with a compatible ethernet shield)
4. Arduino power supply and [Passive PoE Injector]9https://www.littlebirdelectronics.com.au/passive-poe-injector-cable-set) Cable Set
5. Cat6 Cabling, project box, misc cables, screws, weather station roof mounting supplies from your local hardware store.

## Code included in the project/Instructions.

I assume you already have an ELK stack on a Linux box somewhere, and that your arduino is connect to it via the network. Having said that, you don't necesarily need ELK. If you know what you're doing with python, you could just as easily read the JSON and do what you will. I like the ELK stack because (after a steep learning curve) it's easy to create graphs of the data.

The conf.d directory contains the logstash configuration piplines you'll need in order to pull the data in from the solar inverter and arduino weather station. The pipelines.yml file contains the pipelines for logstash.

The Sketches directory contains the sketch you'll need to push to your arduino. You may need to download the latest arduino IDE and the time library which is necessary to get the correct time via the network.