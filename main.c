#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <SendOnlySoftwareSerial.h>
//
//SendOnlySoftwareSerial uart(3);

#define SYS_CLK   1000.0  // [kHz]
#define T2  (((SYS_CLK *4700) /1000000) +1) /4 // >4,7us
#define T4  (((SYS_CLK *4000) /1000000) +1) /4 // >4,0us

// PCF8574A
#define TWIADDR 0x38
#define PP0 0b11111110
#define PP1 0b11111101
#define PP2 0b11111011
#define PP3 0b11110111
#define PP4 0b11101111
#define PP5 0b11011111
#define PP6 0b10111111
#define PP7 0b01111111

// MCU Pin
#define SDA PB0
#define SCL PB2
#define DDR DDRB
#define PORT PORTB
#define InterruptPIN PCINT1
#define LEDTEST PB4

// Define Modes
#define SEND 0
#define GET 1

void Start(void){
    //Starting Communication
    PORT |= (1<<SCL);
    while(!(PINB & (1<<SCL)));
    _delay_us(T2);
    PORT &= ~(1<<SDA);
    _delay_us(T4);
    PORT &= ~(1<<SCL);
    PORT |= (1<<SDA);
}

void Stop(void){
    //End of Communication
    PORT &= ~(1<<SDA);
    PORT |= (1<<SCL);
    while(!(PINB & (1<<SCL)));    // Wait for SCL to go high.
    _delay_us(T4);
    PORT |= (1<<SDA);
    _delay_us(T2);
}

//void Transmit(uint8_t mode) {
void Transmit(void) {
    USISR |= (1<<USIOIF);
    while (!(USISR & (1<<USIOIF))) {
        _delay_us(T2);
        USICR |= (1<<USITC);
        while(!(PINB & (1<<SCL)));
        _delay_us(T4);
        USICR |= (1<<USITC);
    }
    USISR |= (1<<USIOIF);
    _delay_us(T2);
//    USIDR = 0xFF;
//    if(!mode) 
    DDR |= (1<<SDA);
}

unsigned char sendGetData(unsigned char data, unsigned char mode){
    Start();

    // Send SLA-ADDR and Write Bit
    PORT &= ~(1<<SCL);
    if(mode)USIDR = (TWIADDR << 1 | mode); //read
    else USIDR = (TWIADDR << 1); //write
    USISR |= (1<<USISIF)|(1<<USIOIF)|(1<<USIPF);
    Transmit();

    //Reply ACK first, then follow by send
    DDR &= ~(1<<SDA); // SDA in received ACK
    USIDR = 0x00; //ACK ContinueSession
    USISR |= (1<<USICNT3) | (1<<USICNT2) | (1<<USICNT1);
    Transmit();

    // Engage Send / Receive
    if(mode == SEND) {
        //Send Byte Data
        PORT &= ~(1<<SCL);
        USIDR = data;
        USISR |= (1<<USISIF)|(1<<USIOIF)|(1<<USIPF); //8bit
        Transmit();
    } else if(mode == GET) {
        //Receive Byte Data
        DDR &= ~(1 << SDA); // Enable SDA as input
        USISR |= (1<<USISIF)|(1<<USIOIF)|(1<<USIPF); //8bit
        Transmit();
        data = USIDR;
    }

    // Send NACK=FF End
    DDR &= ~(1<<SDA);
    USIDR = 0xFF;       // NACK EndSession
    USISR |= (1<<USICNT3) | (1<<USICNT2) | (1<<USICNT1);//1bit
    Transmit();

    // Stop Send / Get Data Register
    Stop();
    return data;
}

uint8_t DDRPP=0x0F,count;

int main(void){
    unsigned char DDRPPRes=0;
    //unsigned char c=0;
    //OSCCAL -= 10;
    //uart.begin(9600);

    //USI TWI/I2C Init and enable Pin Change Interrupt init
    PORT |= (1<<SDA) | (1<<SCL);
    DDR |= (1<<SDA) | (1<<SCL);
    USICR = (1<<USIWM1) | (1<<USICS1) | (1<<USICLK);
    PCMSK |= 1<<InterruptPIN;
    DDR |= 1<<LEDTEST;
    sendGetData(DDRPP,SEND);

    while (1) {
	    if (GIFR & (1<<PCIF)) {
	        DDRPP = sendGetData(0,GET); //get col val
	        GIFR |= 1<<PCIF;
	    }
	    
	    if(DDRPP > 0 && DDRPP < 0x0F){
	        DDRPPRes = DDRPP; //store col val
	        DDRPP = 0xF0; sendGetData(DDRPP,SEND); //change to row
	        DDRPP = sendGetData(0,GET); //get row val
	        if(DDRPP > 0x0F && DDRPP < 0xF0) DDRPPRes |= DDRPP; //store col+row val

	/* do something here */	
/*	        switch (DDRPPRes){
	            case 119: c=1;break;
	            case 123: c=2;break;
	            case 125: c=3;break;
	            case 183: c=4;break;
	            case 187: c=5;break;
	            case 189: c=6;break;
	            case 215: c=7;break;
	            case 219: c=8;break;
	            case 221: c=9;break;
	            case 235: c=0;break;
	            case 231: c='*';break;
	            case 237: c='#';break;
	            case 126: c='A';break;
	            case 190: c='B';break;
	            case 222: c='C';break;
	            case 238: c='D';break;
	            default: break;
	        }
*/

		//debounce
	        PORT ^= 1<<PB4;
	        DDRPP = 0x0F; sendGetData(DDRPP,SEND); //back
	        _delay_ms(200);
	    }
    }
}
