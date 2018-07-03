#!/usr/bin/python3



#Import libraries
import time
import json
import requests

# Define some variables

#get the date
date = time.strftime("%Y-%m-%d",time.gmtime())
hours = time.strftime("%H",time.gmtime())
minutes = time.strftime("%M",time.gmtime())
seconds = time.strftime("%S",time.gmtime())
#save it as a string
localdt = date+"+"+hours+"%3A"+minutes+"%3A"+seconds

#get the current data from the arduino

request = requests.get(url='10.60.204.181')
print(request.json())