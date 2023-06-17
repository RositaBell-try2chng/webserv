#!/usr/bin/python2.7

import cgi
import os
from datetime import datetime

def printError():
	print("HTTP/1.1 404 Not Found\r\nContent-length: 0\r\n\r\n")

def printFile(filepath):
	extention = os.path.splittext(os.path.basename(filepath.filename))[1].lower()
	if (extention == '.jpg' or extention == '.jpeg' or extention == '.png' or extention == '.bmp'):
		extention[0] = '/'
		AllContent = open(filepath.filename, 'rb').readfile()
		print("HTTP/1.1 200 OK\r\n" + "Content-type: image" + extention + "\r\n\r\n" + AllContent + "\r\n\r\n")

filepath = form['filepath']

if (filepath.fileName):
	printFile(filepath)
else
	printError()

print(open(filepath, "rb").read())