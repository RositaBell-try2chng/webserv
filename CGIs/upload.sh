#!/bin/bash

export SECs=$(date +_%d-%m-%y_%H:%M:%S)

echo "start" >> /home/bars/webserv/logs.txt
echo "upload path = " $UPLOAD_PATH >> /home/bars/webserv/logs.txt
echo "file name = " $FILENAME >> /home/bars/webserv/logs.txt
echo "ext = " $EXTENSION >> /home/bars/webserv/logs.txt


#cat > $UPLOAD_PATH$FILENAME$SECs$EXTENSION

echo -ne "HTTP/1.1 200 OK\r\n"
echo -ne "Content-Type: text/html;charset=utf-8\r\n"
echo -ne "Content-type:text/html\r\n"
echo -ne "<H1> The file has been uploaded </H1>"
echo "end" >> /home/bars/webserv/logs.txt