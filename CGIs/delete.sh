#!/bin/bash

echo -ne "HTTP/1.1 200 OK\r\n"
echo -ne "Content-length: 25\r\n\r\n"
echo -ne "The file has been DELETED")

echo "$FILENAME" > /Users/rbell/schoolproj/webserv/logs.txt
# rm $FILENAME