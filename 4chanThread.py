#4chan Sentiment Analysis
#Python Script to get stuff from 4chan API

import sys
import requests
import re
import html

url = sys.argv[1]

v = 1

pos = url.find("4channel")

if pos == -1:
    v = 2
    pos = url.find("4chan")
    
if pos == -1:
    out = open("thread_info.txt", "w")
    out.write("Error\nInvalid URL")
    out.close()
    sys.exit(1)
    
if v == 1:
    pos += 13
elif v == 2:
    pos += 10
    
board = ""

for i in range(pos, len(url)):
    if url[i]=="/":
        break
    else:
        board += url[i]
        
pos = url.find("thread")

if pos == -1:
    out = open("thread_info.txt", "w")
    out.write("Error\nInvalid URL")
    out.close()
    sys.exit(1)
    
pos += 7

thread_no = ""

for i in range(pos, len(url)):
    if url[i]=="#":
        break
    else:
        thread_no += url[i]
        
thread_link = "https://a.4cdn.org/"+board+"/thread/"+thread_no+".json"

data = requests.get(thread_link)

if data.status_code != 200:
    out = open("thread_info.txt", "w")
    out.write("Error\nHTTP Error: "+data.status_code)
    out.close()
    sys.exit(1)

json_data = data.json()

posts = json_data["posts"]

post = posts[0]

body = "No-Title"
title = "No-Title"
thread_no = post["no"]

if "com" in post:
    body = re.sub("<br>","\n",post["com"])
    body = re.sub("<[^<]+?>","",body)
    body = html.unescape(body)

if "sub" in post:
    title = re.sub("<br>","\n",post["sub"])
    title = re.sub("<[^<]+?>","",title)
    title = html.unescape(title)

out = open("thread_info.txt", "w")
print("Thread_Number:\n", thread_no, sep="", file = out)
print("Title:\n", title, sep="", file = out)
print("Body:\n", body, sep="", file = out)
out.close()
