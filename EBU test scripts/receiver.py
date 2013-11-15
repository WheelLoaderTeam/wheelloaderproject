
# -*- coding: cp1252 -*-
import socket
import os

# mest signifikant till DA är sista byten (högre index), DA tar 16 bitar
# 

# byte 4 bit 1 i relä är solenoid 7 (magnetventilen i testuppställningen)

sockAD = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockAD.bind(("10.0.0.1",25101))
sockIO = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockIO.bind(("10.0.0.1",25301))
sockDA = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockRelay = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#sockRelay.bind(("10.0.0.1",25401))

relays = chr(0)*14
relays = relays[0:13]+chr(0x08) # set relay for A2 to "break"
relaysS07open = relays[0:3]+chr(0x01)+relays[4:14] # set relay for S07 to "break"
sockRelay.sendto(relaysS07open, ("10.0.0.2", 25400))
dataDO = chr(0)*8
#dataDO = dataDO[0:2]+chr(0x40)+dataDO[3:8]
sockIO.sendto(dataDO, ("10.0.0.2", 25300))

toggle = True

# send rekay packet
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

while True:
  (dataIO, addr) = sockIO.recvfrom(1024)
  (dataAD, addr) = sockAD.recvfrom(1024)
  dataNew=""
  i=0
  while i<24:
    dataNew += writeDA(readAD(dataAD, i)*0.5)
    # dataNew += writeDA(3.0)
    i = i+1
  sockDA.sendto(dataNew,("10.0.0.2",25200))

  if readAD(dataAD, 1) > 3:
    dataDO = dataDO[0:2]+chr(0x40)+dataDO[3:8]
    sockIO.sendto(dataDO, ("10.0.0.2", 25300))
  else:
    dataDO = dataDO[0:2]+chr(0x00)+dataDO[3:8]
    sockIO.sendto(dataDO, ("10.0.0.2", 25300))
  
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
    print "A%2i: I=%1.2fV O=%1.2fV"% (i+1, voltage, 0)
    i = i+1
