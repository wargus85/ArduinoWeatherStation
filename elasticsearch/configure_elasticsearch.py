#!/usr/bin/python3
#Code originally from: https://www.elastic.co/blog/arduino-based-home-weather-station-on-the-elastic-stack
#Further ideas from: https://github.com/ultramookie/esweather
import requests
import json

data = {
    "mappings": {
        "reading": {
            "properties": {
                "temperature": {"type": "float"},
                "pressure": {"type": "float"},
                "timestamp": {"type": "date"},
            },
        }
    }
}

response = requests.put('http://localhost:9200/weather', data=json.dumps(data))
try:
    assert response.status_code is 200
except AssertionError:
    raise AssertionError("Your mapping was not created", response)
# You could use this snippet to delete your old data if you have an error.
#response = requests.delete('http://localhost:9200/weather')
#try:
#    assert response.status_code is 200
#except AssertionError:
#    raise AssertionError("Your mapping was not deleted", response)

# // With such a simple document, we're just going to use a string to generate the JSON to send to Elasticsearch
#   String data = "{pressure: "+String(pressure)+", temperature: "+String(temperature)+", timestamp: "+ timestamp +"}";
#   // We can inspect the data being sent over the Serial line, in the Arduino IDE.
#   Serial.println(data);
#   // We now create a URI for the request
#   // This is the index of the Elasticsearch document we're creating
#   String url = "/weather/reading";