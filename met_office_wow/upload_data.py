#!/usr/bin/python3

# This script needs to be run as a cron job on a linux system that has direct tcp/ip access to an arduino weather station
# as detailed on the project page on my blog (links in the readme).
# You will need to add in your own SiteID and Authkey, which are available on your WOW site.
# This script will report the current temperature, wind information, humidity, and rainfall.
# If you run your script as indicated below it will run every 5 minutes. 
# The cumulative rainfall is reset at 9am GMT.

# Installing: Edit your crontab with the following entry:
# */5 * * * * /path/to/script/upload_data.py
# It will run the script every 5 minutes

#Import libraries
import time
import json
import requests

# Define some variables, add your own values
SiteId = "Enter your site ID"
AuthKey = "Enter your authkey"
SoftwareType = "python-linux-v18-07-05.2"
ArduinoURL = "http://192.168.1.5"
BaseURL = "http://wow.metoffice.gov.uk/automaticreading?"

# Get the date
date = time.strftime("%Y-%m-%d",time.gmtime())
hours = time.strftime("%H",time.gmtime())
minutes = time.strftime("%M",time.gmtime())
seconds = time.strftime("%S",time.gmtime())
# Save it as a string in a format that WOW likes
localdt = date+"+"+hours+"%3A"+minutes+"%3A"+seconds

# Request the data from the Arduino and save it to a new variable.
# We aren't doing any error checking, just hoping the data is correct and present.
request = requests.get(url=ArduinoURL)
Data = request.json()

#convert the windspeed in m/s to miles per hour for reporting:
windspeed = Data["wind"]["localspeed"]*2.236936
windgust = Data["wind"]["localgust"]*2.236936
#convert HPa to inches of mercury
baromin = Data["weather"]["pressure"]*0.02953
#convert temperature to f
tempf = (Data["weather"]["temp"]*1.8)+32
#convert mm to in
rainin = (Data["localrain"]["1h"])/25.4
dailyrainin = (Data["localrain"]["24h"])/25.4

# We want to reset the rainfall total every day at 9am, then add the rainfall to that total. 
# The cumulative total is stored in a json encoded file which is read to/from every hour

if int(time.strftime("%M",time.gmtime()))==0: 
    #It's the top of the hour, so we need to add the rainfall to the total (after converting to inches)
    with open('raintotal.json','r+') as openfile:
        RainJson = json.load(openfile)
        RainJson["totalrain"] = RainJson["totalrain"] + rainin
        openfile.seek(0)
        json.dump(RainJson,openfile)
        openfile.truncate()
    dailyrainin = RainJson["totalrain"]
    DataURL = BaseURL+"siteid="+SiteId+"&siteAuthenticationKey="+AuthKey+"&dateutc="+localdt+"&winddir="+str(Data["wind"]["localdeg"])+"&windspeedmph="+str(windspeed)+"&windgustmph="+str(windgust)+"&humidity="+str(Data["weather"]["humidity"])+"&tempf="+str(tempf)+"&baromin="+str(baromin)+"&rainin="+str(rainin)+"&dailyrainin="+str(dailyrainin)+"&softwaretype="+SoftwareType
    
#now check if it is 9am, if so, set the dailyrainin to rainin
elif int(time.strftime("%H",time.gmtime()))==9 and int(time.strftime("%M",time.gmtime()))==0:
    DataURL = BaseURL+"siteid="+SiteId+"&siteAuthenticationKey="+AuthKey+"&dateutc="+localdt+"&winddir="+str(Data["wind"]["localdeg"])+"&windspeedmph="+str(windspeed)+"&windgustmph="+str(windgust)+"&humidity="+str(Data["weather"]["humidity"])+"&tempf="+str(tempf)+"&baromin="+str(baromin)+"&rainin="+str(rainin)+"&dailyrainin="+str(rainin)+"&softwaretype="+SoftwareType
    # write to the file
    with open('raintotal.json','r+') as writefile:
        RainJson = json.load(writefile)
        RainJson["totalrain"] = rainin
        writefile.seek(0)
        json.dump(RainJson,writefile)
        writefile.truncate()
else:
    #just open the file and read the rain, don't edit it.
    with open('raintotal.json','r') as openfile:
        RainJson = json.load(openfile)
    dailyrainin = RainJson["totalrain"]
    DataURL = BaseURL+"siteid="+SiteId+"&siteAuthenticationKey="+AuthKey+"&dateutc="+localdt+"&winddir="+str(Data["wind"]["localdeg"])+"&windspeedmph="+str(windspeed)+"&windgustmph="+str(windgust)+"&humidity="+str(Data["weather"]["humidity"])+"&tempf="+str(tempf)+"&baromin="+str(baromin)+"&rainin="+str(rainin)+"&dailyrainin="+str(dailyrainin)+"&softwaretype="+SoftwareType


#print(DataURL)
#WOW expects a GET to the full url
requestWow = requests.get(url=DataURL)
