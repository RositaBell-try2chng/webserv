#!/usr/local/bin/python3.9

import os
import sys
from datetime import datetime

def printError(str):
	print("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html;charset=utf-8\r\n\r\n", end = '')
	print("<H1>", str, "doesn't set", "</H1>", end = '')

curTS = datetime.now().strftime("_%d-%m-%Y:%H:%M:%S:%f")

fileName = os.environ.get('FILENAME')
ext = os.environ.get('EXTENSION')
uploadPath = os.environ.get('UPLOAD_PATH')

if (fileName == None):
	printError('FILENAME')
elif (ext == None):
	printError('EXTENSION')
elif (uploadPath == None):
	printError('UPLOAD_PATH')
else:
    try:
	    os.mkdir(uploadPath)
    except:
        None
    finally:
        dstFileName = uploadPath + fileName + curTS + ext

        dst = open(dstFileName, 'wb')
        src = open(0, 'rb')

        allContent = src.read(10000)
        while len(allContent) == 10000:
            dst.write(allContent)
            allContent = src.read(10000)
        dst.write(allContent)

        dst.close()
        src.close()
        print("HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8", end = '')
        print("Content-type:text/html\r\n\r\n", end = '')
        print("<H1>", fileName + curTS + ext, "has been uploaded </H1>", end = '')
        exit(0)