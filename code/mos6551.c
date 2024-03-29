#include "mos6551.h"
#include "parser.h"

#define ACIA_RXD (*(volatile uint8_t*)0x6000)  //ACIA receive data port
#define ACIA_TXD (*(volatile uint8_t*)0x6000)  //ACIA transmit data port
#define ACIA_STS (*(volatile uint8_t*)0x6001)  //ACIA status port
#define ACIA_RES (*(volatile uint8_t*)0x6001)  //ACIA reset port
#define ACIA_CMD (*(volatile uint8_t*)0x6002)  //ACIA command port
#define ACIA_CTL (*(volatile uint8_t*)0x6003)  //ACIA control port

volatile char mos6551_RxChar;
volatile char mos6551_rxrb[256];		//DO NOT CHAGE! IT NEEDS TO BE 256 BYTES LONG!
volatile uint8_t mos6551_rxrb_head = 0;
volatile uint8_t mos6551_rxrb_tail = 0;
char mos6551_line[64];				
uint8_t mos6551_line_ind = 0;


void __fastcall__ mos6551_init (void) {
	//initialise 6551 ACIA
    ACIA_RES = 0xFF;	//soft reset (value not important)
//    ACIA_CMD = 0x0B;    //set specific modes and functions: no parity, no echo, no Tx interrupt, no Rx interrupt, enable Tx/Rx
    ACIA_CMD = 0x09;     //set specific modes and functions: no parity, no echo, no Tx interrupt, Rx interrupt enabled, enable Tx/Rx  
//    ACIA_CTL = 0x18;    //8-N-1, 1200 baud
    ACIA_CTL = 0x1E;    //8-N-1, 9600 baud
}


void __fastcall__ mos6551_putc (char c) {
	while (!(ACIA_STS & 0x10));
	ACIA_TXD = c;
}


void __fastcall__ mos6551_puts (const char *str) {
	while (*str != '\0') {
		mos6551_putc(*str);
		str++;
	}
}

void __fastcall__ mos6551_handle_rx (void) {
	while (mos6551_rxrb_head != mos6551_rxrb_tail) {			// There is a new data in ring buffer
		mos6551_RxChar = mos6551_rxrb[mos6551_rxrb_tail];
		mos6551_rxrb_tail++;
		switch(mos6551_RxChar) {
			case 0:  break;			// ignorujemy znak \0
			case 13: break;			// ignorujemy znak CR
			
			case 10:
			mos6551_line[mos6551_line_ind] = '\0';
			mos6551_line_ind = 0;
			parse_cmd(mos6551_line);
			break;
			
			default:
			mos6551_line[mos6551_line_ind] = mos6551_RxChar;
			mos6551_line_ind++;
			if ( mos6551_line_ind >= sizeof(mos6551_line) ) mos6551_line_ind = 0;		//Not needed if buffer size is 256
			break;
		}	
	}
}
