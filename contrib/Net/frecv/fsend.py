#!/c/Python25/python.exe

import sys

argv = sys.argv
if(len(argv) != 2):
	print 'usage: fsend.py PATH'
	exit


import socket

soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
	soc.connect(('localhost',8080))
	
	try :
		f = open(argv[1], "rb")
		while 1 :
			s = f.read(1024)
			if s == "" :
				break
			soc.sendall(s)
		f.close()
		print "[send done]"
	except :
		print "file error."
finally:
	soc.close()

