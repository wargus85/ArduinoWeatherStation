#!/usr/bin/python3

import requests
import json
data = {
    "mappings": {
        "doc": {
            "properties": {
                "coord": {"type": "geo_point"},
                "coordlocal": {"type": "geo_point"},
                "dt": {"type": "date"},
                "localdt": {"type": "date"},
                "sys.sunrise": {"type": "date"},
                "sys.sunset": {"type": "date"},
                "clouds.all": {"type": "float"},
                "main.humidity": {"type": "float"},
                "main.temp": {"type": "float"},
                "main.pressure": {"type": "float"},
                "rain.3hr": {"type": "float"},
                "visibility": {"type": "float"},
                "weather.humidity": {"type": "float"},
                "weather.temp": {"type": "float"},
                "weather.pressure": {"type": "float"},
                "wind.deg": {"type": "float"},
                "wind.speed": {"type": "float"},
                "wind.localspeed": {"type": "float"},
                "wind.localdeg": {"type": "float"},
                "wind.localgust": {"type": "float"},
                "localrain.1hr": {"type": "float"},
                "localrain.24hr": {"type": "float"},
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