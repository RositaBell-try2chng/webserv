import cgi
import os
from datetime import datetime

filepath = form['filepath']

print(open(filepath, "rb").read())