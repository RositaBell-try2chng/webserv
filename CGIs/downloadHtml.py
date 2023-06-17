#!/usr/bin/python2.7

import cgi
import os
from datetime import datetime

filepath = form['filepath']

AllContent = open(filepath, "r").readlines()

print("HTTP/1.1 200 OK\r\n" + "Content-type: text/html\r\n" + "Content-length: " + AllContent.length + "\r\n\r\n" + AllContent + "\r\n\r\n")