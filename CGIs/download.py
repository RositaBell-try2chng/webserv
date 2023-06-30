#!/usr/local/bin/python3.9

import os

extensionsImage = ["jpeg", "png", "jpg"]
extensionIcon = ["ico"]

fileToGet = os.environ.get('FILENAME')
startString = os.environ.get('RESPONSE_START_STRING')
AllContent = open(fileToGet, 'rb').read()
extension = os.path.splitext(fileToGet)[1]
extension = extension[1:]
if extension in extensionsImage:
	stdout = open(1, 'wb')
	stdout.write(("HTTP/1.1 200 OK\r\n" + "Content-type: image/" + extension + "\r\n\r\n").encode())
	stdout.write(AllContent)
elif extension in extensionIcon:
	print("HTTP/1.1 404 Not Found\r\n\r\n", end='')
else:
	if (startString == None):
		print("HTTP/1.1 200 OK\r\n" + "Content-type: text/" + extension + "\r\n\r\n" + AllContent.decode(), end='')
	else:
		print(startString + "Content-type: text/" + extension + "\r\n\r\n" + AllContent.decode(), end='')
