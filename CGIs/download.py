#!/usr/bin/python2.7

from __future__ import print_function
import cgi
import os


extentionsImage = ["jpeg", "png", "jpg"]

def printError():
	print("HTTP/1.1 500 Internal Server Error\r\nContent-length: 0", end = '')

def printFile(fileToGet):
	AllContent = open(fileToGet, 'rb').read()
	extention = os.path.splitext(fileToGet)[1]
	extention = extention[1:]
	if (extention in extentionsImage):
		print("HTTP/1.1 200 OK\r\n" + "Content-type: image/" + extention + "\r\n\r\n" + AllContent, end = '')
	else:
		print("HTTP/1.1 200 OK\r\n" + "Content-type: text/" + extention + "\r\n\r\n" + AllContent)


fileToGet = os.environ.get('FILENAME')

if (fileToGet != None):
	printFile(fileToGet)
else:
	printError()