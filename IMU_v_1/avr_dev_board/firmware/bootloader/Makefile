CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=at90can128
OBJ2HEX=avr-objcopy 
UISP=/usr/local/bin/uisp
SOURCES = main.c usart.c bootloader.c flash/flash_boot_drv.c flash/flash_boot_lib.c
OBJS = $(SOURCES:.c=.o)
TARGET = main

all: $(SOURCES) $(TARGET).hex

$(TARGET).hex : $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET).exe
# Bootloader section starts at 0x1E000
	$(CC) $(CFLAGS) -Wl,--section-start=.text=0x1E000 $(OBJS) -o $(TARGET).exe
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET).exe $@

%.obj : %.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f *.hex *.obj *.o *.exe flash/*.o

# avrdude has some permission issue with the USB, sudo resolves this. Remove sudo if using windows.
program :
	sudo avrdude -c avrispmkII -P usb -p c128 -B 10 -V -u -U flash:w:main.hex

# If some other fuse bits must be set or cleared in the high fuse register it has to be
# managed manually.
program_hfuse :
	sudo avrdude -c avrispmkII -P usb -p c128 -V -u -B 100 -U hfuse:w:0x90:m

read_program :
	sudo avrdude -c avrispmkII -P usb -p c128 -u -U -B 100 flash:r:read_main.hex:i
