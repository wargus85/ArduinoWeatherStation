#!/usr/bin/python3



#Import libraries
import time
import json
import requests

# Define some variables

#testData = {'coordlocal': {'lat': -32.0, 'lon': 115.86}, 'weather': {'temp': 17.78, 'pressure': 1010.7, 'humidity': 51}, 'localrain': {'24h': 10, '1h': 0}, 'localdt': 1530601625, 'wind': {'cardinal': 'SW', 'localspeed': 1.79, 'localdeg': 225, 'localgust': 4.02}}

#get the date
date = time.strftime("%Y-%m-%d",time.gmtime())
hours = time.strftime("%H",time.gmtime())
minutes = time.strftime("%M",time.gmtime())
seconds = time.strftime("%S",time.gmtime())
#save it as a string
localdt = date+"+"+hours+"%3A"+minutes+"%3A"+seconds

#get the current data from the arduino

request = requests.get(url='http://10.60.204.181')
print(request.json()["wind"]["localdeg"])


#print(testData)
