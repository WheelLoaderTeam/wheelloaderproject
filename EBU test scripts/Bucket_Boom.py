# -*- coding: cp1252 -*-
import threading
import serial
import struct
import socket
import os
import signal
import sys

global bucket
global boom
bucket = True
boom   = False

def signal_handler(signal, frame):
    print 'Switching between boom/bucket'
    global bucket
    global boom
    bucket = not bucket
    boom   = not boom
signal.signal(signal.SIGINT, signal_handler)

print 'Press Ctrl+C To switch between boom bucket'



# mest signifikant till DA är sista byten (högre index), DA tar 16 bitar
# 
EBUIP = "10.0.0.3"
# byte 4 bit 1 i relä är solenoid 7 (magnetventilen i testuppställningen)

sockAD = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockAD.bind(("10.0.0.1",25101))
sockIO = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockIO.bind(("10.0.0.1",25301))
sockDA = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockRelay = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#sockRelay.bind(("10.0.0.1",25401))


relays = chr(0)*14
relays = relays[0:11]+chr(0x09)+chr(0x12)+relays[13:14] # set relays for A9, A10, A11, A12 to "break"
sockRelay.sendto(relays, (EBUIP, 25400))



dataDO = chr(0)*8
#dataDO = dataDO[0:2]+chr(0x40)+dataDO[3:8]
sockIO.sendto(dataDO, (EBUIP, 25300))

toggle = True

joystick = 0

class DataCaptureThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        print 'Second thread started'
        DataCapture()

def data2voltage(data):
    value = 0
    for char in data:
        value = value*256 + ord(char)
    
    return float(value)/210

def DataCapture():
    global joystick

    ser = serial.Serial(
        port='COM19',\
        baudrate=19200,\
        parity=serial.PARITY_NONE,\
        stopbits=serial.STOPBITS_ONE,\
        bytesize=serial.EIGHTBITS,\
        timeout=0)

    line = ''
    
    while True:
        for char in ser.read():
            #print char
            if char == '\n':
                joystick = data2voltage(line)
                line = ''
            else:
                line = char + line
    ser.close()





# send relay packet
def setRelays(data):
  none

# convert data from AD at position twoByteIndex (indexes two-bytes, since AD data are two bytes)
# to voltage, assuming a reference maximum of 5 volts
def readAD(data, twoByteIndex):
  return float( ord(data[twoByteIndex*2+1])*256 + ord(data[twoByteIndex*2]) ) / 4096 * 5

# converts from voltage to DA value (16-bit)
# most significant byte is the highest indexed byte (last byte)
def writeDA(voltage):
  if voltage>=5:
    voltage = 65535.0/65536*5 # as close to 5 as possible
  if voltage <0:
    voltage = 0.0
  intValue = int (voltage/5 * 65536)
  msbChar = chr( ((intValue >> 8) & 0xFF) )
  lsbChar = chr( intValue & 0xFF )
  return lsbChar + msbChar

def convertADstringToFloatList(ADdata):
  floats = []
  i = 0
  while i<24:
    floats += readAD(ADdata, i)
  return floats

def convertFloatListToDAstring(floatList):
  daString = ""
  i = 0
  while i<24:
    daString += writeDa(floatList[i])
  return floats


# Start data capture thread
print 'Program start'
t = DataCaptureThread()
t.start()

print 'first thread running'

while True:
  (dataIO, addr) = sockIO.recvfrom(1024)
  (dataAD, addr) = sockAD.recvfrom(1024)
  dataNew=""
  i=0
  while i<24:
    # dataNew += writeDA(readAD(dataAD, i)*0.5)
    if ((i==8) or (i==9) and boom): #(Relay Channel #) - (1)
        dataNew += writeDA(joystick)	
    if ((i==10) or (i==11) and bucket): #(Relay Channel #) - (1)
        dataNew += writeDA(joystick)
    else:
        #dataNew += writeDA(readAD(dataAD, i))
        dataNew += writeDA(1.0)
    i = i+1
  sockDA.sendto(dataNew,(EBUIP,25200))

  if readAD(dataAD, 1) > 3:
    dataDO = dataDO[0:2]+chr(0x40)+dataDO[3:8]
    sockIO.sendto(dataDO, (EBUIP, 25300))
  else:
    dataDO = dataDO[0:2]+chr(0x00)+dataDO[3:8]
    sockIO.sendto(dataDO, (EBUIP, 25300))
  
##  if toggle:
##    toggle = False
##    sockRelay.sendto(relaysS07open, ("10.0.0.2", 25400))
##  else:
##    toggle = True
##    sockRelay.sendto(relays, ("10.0.0.2", 25400))
  os.system('cls')
  i=0
  while i<len(dataAD)/2:
    voltage = readAD(dataAD, i)
    print "A%2i: I=%1.2fV Joystick=%1.2fV"% (i+1, voltage, joystick)
    i = i+1

