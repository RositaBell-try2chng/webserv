#!/bin/bash

rm $FILENAME

if [ $? == 0 ]
then
    echo -ne "HTTP/1.1 200 OK\r\n"
    echo -ne "Content-Type: text/txt\r\n\r\n"
    echo -ne "The file has been DELETED"
else
    echo -ne "HTTP/1.1 500 Internal Server Error\r\n"
    echo -ne "Content-Type: text/txt\r\n\r\n"
    echo -ne "The DELETE has been failed"
fi
