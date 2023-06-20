#!/usr/bin/python2.7

import cgi, os
from datetime import datetime

curTS = datetime.now().strftime("%d-%m-%Y:%H:%M:%S:%MS")

form = cgi.FieldStorage()

# get file
fileitem = form['filename']

# is file in the form?
if fileitem.filename:
    open(os.getcwd() + '/cgi-bin/' + os.path.basename(fileitem.filename) + " " + curTS, 'wb').write(fileitem.file.read())
    msg = 'The file "' + os.path.basename(fileitem.filename) + '" has been uploaded.'
else:
    msg = 'Have no file in form'

print("Content-Type: text/html;charset=utf-8")
print("Content-type:text/html\r\n")
print("<H1> " + msg + " </H1>")