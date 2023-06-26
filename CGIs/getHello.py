#!/usr/local/bin/python3.9

print("HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n", end = "")
print("<!DOCTYPE html>\r\n")
print('<html>\r\n')
print('<head>\r\n')
print('<meta http-equiv="content-type" content="text/html; charset=UTF-8">\r\n')
print('<title>getHello.py</title>\r\n')
print('<link rel="shortcut icon" href="data:," />\r\n')
print('</head>\r\n')
print('<body>\r\n')
print('<div>Hello World!</div>\r\n')
print('</body>\r\n')
print('</html>')