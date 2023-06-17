#!/bin/bash

export SECs=$(date +_%d-%m-%y_%H:%M:%S)
export NEWFILENAME="$FILENAME$SECs$EXTENSION"

cat > $NEWFILENAME

echo -n "HTTP/1.1 200 OK\r\n"
echo -n "Content-Type: text/html;charset=utf-8\r\n"
echo -n "Content-type:text/html\r\n"
echo -n "Content-length: 41\r\n\r\n"
echo -n "<H1> The file has been uploaded </H1>\r\n\r\n")