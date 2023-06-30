#!/usr/local/bin/python3.9

import os
import sys
from datetime import datetime

extensionsImage = ["jpeg", "png", "jpg"]

def printError(str):
    print("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html;charset=utf-8\r\n\r\n", end='')
    print("<H1>", str, "doesn't set", "</H1>", end='')


curTS = datetime.now().strftime("_%d-%m-%Y:%H:%M:%S:%f")

fileName = os.environ.get('FILENAME')
ext = os.environ.get('EXTENSION')
uploadPath = os.environ.get('UPLOAD_PATH')

if fileName is None:
    printError('FILENAME')
elif ext is None:
    printError('EXTENSION')
elif uploadPath is None:
    printError('UPLOAD_PATH')
else:
    root = os.getcwd()

    try:
        os.mkdir(uploadPath)
    except:
        None
    finally:
        fileName += curTS + ext

        dst = open(uploadPath + fileName, 'wb')
        src = open(0, 'rb')

        allContent = src.read(10000)
        while len(allContent) == 10000:
            dst.write(allContent)
            allContent = src.read(10000)
        dst.write(allContent)

        dst.close()
        src.close()

        os.chdir(uploadPath)
        dirToWrite = os.path.basename(os.getcwd())

        stdout = open(1, 'wb')
        stdout.write("HTTP/1.1 201 OK\r\n".encode())
        stdout.write(("Location: " + uploadPath + " \r\n").encode())
        stdout.write("Content-Type: text/html;charset=utf-8\r\n\r\n".encode())
        stdout.write(("<H1> " + fileName + " has been uploaded to " + dirToWrite + " </H1>").encode())
        src = os.getcwd().replace(root, '') + '/'
        if ext[1:] in extensionsImage:
            # stdout.write(("<H1> " + src + fileName + " </H1>").encode())
            stdout.write(("<img src=" + src + fileName + ">").encode())
        else:
            stdout.write("<dev> ".encode())
            stdout.write(allContent)
            stdout.write(" </dev>".encode())
