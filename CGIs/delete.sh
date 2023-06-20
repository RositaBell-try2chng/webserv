#!/bin/bash

echo -n "HTTP/1.1 200 OK\r\n"
echo -n "Content-length: 25\r\n\r\n"
echo -n "The file has been DELETED")

rm $FILENAME