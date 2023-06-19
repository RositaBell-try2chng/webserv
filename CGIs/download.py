#!/usr/bin/python2.7

import cgi
import os

def printError():
	print("HTTP/1.1 500 Internal Server Error\r\nContent-length: 0\r\n\r\n")
	exit(1)

def printFile(fileToGet):
	AllContent = open(fileToGet, 'rb').readfile()
	print("HTTP/1.1 200 OK\r\n" + "Content-type: image" + extention + "\r\n\r\n" + AllContent)


fileToGet = os.environ.get('FILENAME')

if (fileToGet != None):
	printFile(fileToGet)
else
	printError()