GCC_MCU=attiny85
FUSE_L=0xE2
FUSE_H=0xD7
F_CPU=1000000
AVRDUDE_MCU=t85
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${GCC_MCU} -DF_CPU=${F_CPU} -I. 
TARGET=main
SRCS = main.c
PROGRAMMER=usbasp
PORT=usb
BAUD=57600
#CLOCK=-B10
#PREVENT_ERASE=-D

all:
	${CC} ${CFLAGS} -o ${TARGET}.o ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.o ${TARGET}.hex
	${SIZE} -C --mcu=${GCC_MCU} ${TARGET}.o
flash:
	${AVRDUDE} -p${AVRDUDE_MCU} -P${PORT} -b${BAUD} ${PREVENT_ERASE} -c${PROGRAMMER} -Uflash:w:${TARGET}.hex:i
info:
	${AVRDUDE} -p${AVRDUDE_MCU} -c${PROGRAMMER}
fuse:
	$(AVRDUDE) -p${AVRDUDE_MCU} -c${PROGRAMMER} -Uhfuse:w:${FUSE_H}:m -Ulfuse:w:${FUSE_L}:m
clean:
	rm -f *.c~ *.h~ *.o *.hex
