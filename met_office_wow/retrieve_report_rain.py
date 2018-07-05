#!/usr/bin/python3

import requests
import time
import json

# date format in the json we pass to the query must be in the format of:
# 2018-07-04T09:00:00
# 24 hours ago is 86400 seconds
#get the epoc in seconds
#yesterday = time.time()-86400

# If 9am GMT, report the dailyrainin at 0.
# if 


#I need to get the rainfall at 
today = time.time()
date = time.strftime("%Y-%m-%dT09:00:00",time.gmtime(today)) #will return the time at 9am today


#GET weather/_search 
query = {
    "query": {
        "range" : {
            "@timestamp" : {
                "gte" : "2018-07-04T09:00:00",
                "lt" : "2018-07-04T09:00:10"
            }
        }
    }
}