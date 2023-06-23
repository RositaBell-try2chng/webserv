#!/bin/bash

echo -ne "HTTP/1.1 200 OK\r\n"
echo -ne "Content-length: 12\r\n\r\n"
echo -ne "Hello World!"