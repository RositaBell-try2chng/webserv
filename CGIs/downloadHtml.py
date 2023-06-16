import cgi
import os
from datetime import datetime

filepath = form['filepath']

AllContent = open(filepath, "r").readlines()

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")
print("Content-length: " + AllContent.length "\r\n")
