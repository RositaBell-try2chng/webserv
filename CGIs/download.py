#!/usr/local/bin/python3.9

import os

extentionsImage = ["jpeg", "png", "jpg"]
extentionIcon = ["ico"]

fileToGet = os.environ.get('FILENAME')
startString = os.environ.get('RESPONSE_START_STRING')
AllContent = open(fileToGet, 'rb').read()
extention = os.path.splitext(fileToGet)[1]
extention = extention[1:]
if (extention in extentionsImage):
	stdout = open(1, 'wb')
	stdout.write(("HTTP/1.1 200 OK\r\n" + "Content-type: image/" + extention + "\r\n\r\n").encode())
	stdout.write(AllContent)
elif (extention in extentionIcon):
	print("HTTP/1.1 404 Not Found\r\n\r\n", end = '')
else:
	if (startString == None):
		print("HTTP/1.1 200 OK\r\n" + "Content-type: text/" + extention + "\r\n\r\n" + AllContent.decode(), end = '')
	else:
		print(startString + "Content-type: text/" + extention + "\r\n\r\n" + AllContent.decode(), end = '')