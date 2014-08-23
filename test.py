#!/usr/bin/env python

from xbee import ZigBee
import serial
import time

def print_data(data):
	print data

ser = serial.Serial('/dev/tty.usbserial-AH0014VV', 9600)
xbee = ZigBee(ser, callback=print_data)

xbee.tx(
	dest_addr_long='\x00\x13\xA2\x00\x40\x31\x56\x46',
	dest_addr = b'\x10\x52',
	data=b'\x01',
	)

while True:
	try:
		time.sleep(0.001)
	except KeyboardInterrupt:
		break

xbee.halt()
ser.close()