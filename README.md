# USI TWI/I2C examples
- Attiny USI TWI/I2C simple examples and PCF8574 GPIO expander. 
- Very small compared to Arduino version because its directly coded using the RISC Instruction Set MCU Registers.

# Tools
1. Software
- AVRDdude Programmer
- AVR-GCC
- Linux OpenSuSE Tumbleweed
- Nick Gammon SendOnlySoftwareSerial Library (https://github.com/nickgammon/SendOnlySoftwareSerial)

2. Hardware
- USBASP
- Attiny85
- PCF8574
- Keypad 4x4 Membrane Matrix
- USB to Serial (for debugging)


# build
- You might one to check the Makefile first before running make.
- And run `make` and `make flash`

```
affan@tumbleweed:~/uC/tiny85/c/T85-USI-PCF8574> make
avr-gcc -std=c99 -Wall -g -Os -mmcu=attiny85 -DF_CPU=1000000 -I.  -o main.o main.c
/usr/lib64/gcc/avr/11/ld: warning: -z relro ignored
avr-objcopy -j .text -j .data -O ihex main.o main.hex
avr-size -C --mcu=attiny85 main.o
AVR Memory Usage
----------------
Device: attiny85

Program:     372 bytes (4.5% Full)
(.text + .data + .bootloader)

Data:          3 bytes (0.6% Full)
(.data + .bss + .noinit)
```
