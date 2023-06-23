#!/usr/bin/python3

import cgi
import os


extentionsImage = ["jpeg", "png", "jpg"]

fileToGet = os.environ.get('FILENAME')
AllContent = open(fileToGet, 'rb').read()
extention = os.path.splitext(fileToGet)[1]
extention = extention[1:]
if (extention in extentionsImage):
	print("HTTP/1.1 200 OK\r\n" + "Content-type: image/" + extention + "\r\n\r\n" + AllContent.decode(), end = '')
else:
	print("HTTP/1.1 200 OK\r\n" + "Content-type: text/" + extention + "\r\n\r\n" + AllContent.decode(), end = '')