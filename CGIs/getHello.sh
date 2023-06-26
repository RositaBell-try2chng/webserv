#!/bin/bash

echo -ne "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n"
echo -ne "<!DOCTYPE html>\r\n"
echo -ne '<html>\r\n'
echo -ne '<head>\r\n'
echo -ne '<meta http-equiv="content-type" content="text/html; charset=UTF-8">\r\n'
echo -ne '<title>getHello.py</title>\r\n'
echo -ne '<link rel="shortcut icon" href="data:," />\r\n'
echo -ne '</head>\r\n'
echo -ne '<body>\r\n'
echo -ne '<div>Hello World!</div>\r\n'
echo -ne '</body>\r\n'
echo -ne '</html>'