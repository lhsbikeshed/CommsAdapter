import serial
from time import sleep
charMap = ['g', '#', 'f', 'e', '#', 'c', 'b', 'a', 't', 's', 
		  'r', 'q', 'p', 'o', 'n', 'm', 'g', 'h', 'l', '5', 
		  '4', '3', '2', '1', 'k', 'i', 'j', '6', '0', '*', 
		  '9', '8']


s = serial.Serial('/dev/ttyACM0')

while(False):
	r = s.read()
	print r, ord(r) - 65, charMap[ord(r) - 65]

a=0
ct = 0
while(True):
	ct+=1
	ct %= 32
	a = 1 << ct
	s.write( chr((a >> 24) & 0xFF ))
	s.write( chr((a >> 16) & 0xFF ))	
	s.write( chr((a >> 8) & 0xFF ))
	s.write( chr((a ) & 0xFF ))
	s.write(',')
	print ct
	sleep(0.1)
