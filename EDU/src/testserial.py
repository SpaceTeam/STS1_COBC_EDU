#!/usr/bin/env python   
      
import time
import serial
               
ser = serial.Serial(            
     port='/dev/serial0',
     baudrate = 9600,
     parity=serial.PARITY_NONE,
     stopbits=serial.STOPBITS_ONE,
     bytesize=serial.EIGHTBITS
)

while 1:
    char = ser.read(1)  # 1 byte
    print(char)
    ser.write(char + b' was received\r\n')
