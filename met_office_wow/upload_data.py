#!/usr/bin/python3

# This script needs to be run as a cron job on a linux system that has direct tcp/ip access to an arduino weather station
# as detailed on the project page on my blog (links in the readme).
# You will need to add in your own SiteID and Authkey, which are available on your WOW site.
# This script will report the current temperature, wind information, humidity, and rainfall.
# Only run this script once an hour, otherwise it will over-report the rainfall information.

# Installing: Edit your crontab with the following entry:
# */5 * * * * /path/to/script/upload_data.py
# It will run the script every 5 minutes

#Import libraries
import time
import json
import requests

# Define some variables (you will need to edit the url to your arduino too, below)
SiteId = "Enter your site ID"
AuthKey = "Enter your authkey"
SoftwareType = "python-linux-v18-07-04.1"
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
# Edit the url to point to your arduino
request = requests.get(url='http://10.60.204.181')
Data = request.json()

#convert the windspeed in m/s to mph for reporting:
windspeed = Data["wind"]["localspeed"]*2.236936
windgust = Data["wind"]["localgust"]*2.236936
#convert HPa to inches of mercury
baromin = Data["weather"]["pressure"]*0.02953
#convert temperature to f
tempf = (Data["weather"]["temp"]*1.8)+32
#convert mm to in
rainin = (Data["localrain"]["1h"])/25.4
dailyrainin = (Data["localrain"]["24h"])/25/4

# We only want to report the rain every hour and the total rain at 6am every day, otherwise don't report the rain.

if int(time.strftime("%M",time.gmtime()))==0:
    DataURL = BaseURL+"siteid="+SiteId+"&siteAuthenticationKey="+AuthKey+"&dateutc="+localdt+"&winddir="+str(Data["wind"]["localdeg"])+"&windspeedmph="+str(windspeed)+"&windgustmph="+str(windgust)+"&humidity="+str(Data["weather"]["humidity"])+"&tempf="+str(tempf)+"&baromin="+str(baromin)+"&rainin="+str(rainin)+"&softwaretype="+SoftwareType
    #now check if it is 6am
    if int(time.strftime("%H",time.gmtime()))==6:
            DataURL = BaseURL+"siteid="+SiteId+"&siteAuthenticationKey="+AuthKey+"&dateutc="+localdt+"&winddir="+str(Data["wind"]["localdeg"])+"&windspeedmph="+str(windspeed)+"&windgustmph="+str(windgust)+"&humidity="+str(Data["weather"]["humidity"])+"&tempf="+str(tempf)+"&baromin="+str(baromin)+"&rainin="+str(rainin)+"&dailyrainin="+str(dailyrainin)+"&softwaretype="+SoftwareType
else:
    DataURL = BaseURL+"siteid="+SiteId+"&siteAuthenticationKey="+AuthKey+"&dateutc="+localdt+"&winddir="+str(Data["wind"]["localdeg"])+"&windspeedmph="+str(windspeed)+"&windgustmph="+str(windgust)+"&humidity="+str(Data["weather"]["humidity"])+"&tempf="+str(tempf)+"&baromin="+str(baromin)+"&softwaretype="+SoftwareType

#print(testData)
print(DataURL)

#WOW expects a GET to the full url

requestWow = requests.get(url=DataURL)
