#!/usr/local/bin/python3.9

import cgi
import os
import sys


extentionsImage = ["jpeg", "png", "jpg"]
extentionIcon = ["ico"]

fileToGet = os.environ.get('FILENAME')
AllContent = open(fileToGet, 'rb').read()
extention = os.path.splitext(fileToGet)[1]
extention = extention[1:]
if (extention in extentionsImage):
	stdout = open(1, 'wb')
	stdout.write(("HTTP/1.1 200 OK\r\n" + "Content-type: image/" + extention + "\r\n\r\n").encode())
	stdout.write(AllContent)
elif (extention in extentionIcon):
	print("HTTP/1.1 200 OK\r\n", end = '')
else:
	print("HTTP/1.1 200 OK\r\n" + "Content-type: text/" + extention + "\r\n\r\n" + AllContent.decode(), end = '')