#!/c/Python25/python.exe

import sys

argv = sys.argv
if(len(argv) != 2):
	print 'usage: fsend.py PATH'
	exit


import socket

soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
	soc.connect(('localhost',8181))
	
	try :
		soc.sendall(argv[1])
		print "[send done]"
	except :
		print "send error."
finally:
	soc.close()

