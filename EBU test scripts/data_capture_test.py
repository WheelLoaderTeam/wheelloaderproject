import serial
import struct

ser = serial.Serial(
    port='COM4',\
    baudrate=19200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

print("connected to: " + ser.portstr)
count=1
i = 0
ourvalue = ''
line = ''

def data2voltage(data):
    value = 0
    for char in data:
        value = value*256 + ord(char)
    
    return float(value)/180


while True:

    
##    for i in range(0,4):
##        for line in ser.read():
##            if i<3:
##                ourvalue = ourvalue + line
##            if i == 3:
##                print ourvalue
##                ourvalue = ''

    
    for char in ser.read():
        #print char
        if char == '\n':
            print data2voltage(line)
            line = ''
        else:
            line = char + line
##            print line
        



##    # read 2 bytes from hardware as a string
##    s = ser.read(2)
##    # h means signed short
##    # < means "little-endian, standard size (16 bit)"
##    # > means "big-endian, standard size (16 bit)"
##    value = struct.unpack("<h", s) # hardware returns little-endian
##    value = struct.unpack(">h", s) # hardware returns big-endian
##    print value

ser.close()
