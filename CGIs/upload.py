#!/usr/bin/python2.7

import cgi
import os
from datetime import datetime

curTS = datetime.now().strftime("%d-%m-%Y:%H:%M:%S:%MS")
form = cgi.FieldStorage()
fileitem = form['filename']
uploadPath = form.getfirst('upload_path', 'None')

fileName = upload_path + os.path.basename(fileitem.filename) + " "  + curTS

if fileitem.filename or upload_path == 'None':
   open(upload_path + fileName, 'wb').write(fileitem.file.read())
   toSend = 'The file "' + fileName + '" has been uploaded to ' + upload_path
else:
   toSend = 'Uploading Failed'

print("Content-Type: text/html;charset=utf-8\r\nContent-type:text/html\r\n<H1> " + toSend + " </H1>")