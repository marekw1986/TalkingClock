#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <6502.h>
#include "delay.h"
#include "m6242.h"
#include "mc6840.h"
#include "mos6551.h"
#include "spo256.h"
#include "io.h"

//0x6000 - CS0
//		0x6000 - UART
//		0x6100 - TIMER
//		0x6200 - IN PORT (status flags)
//		0x6300 - SP0
//0x6400 - CS1		DISPLAY BOARD
//		0x6400 - RTC
//		0x6600 - DIG0 (write) / BUTTONS (read)
//		0x6601 - DIG1 (write)
//		0x6602 - DIG2 (write)
//		0x6603 - DIG3 (write)
//		0x6680 - NOT USED
//		0x6700 - NOT USED
//		0x6780 - NOT USED

//0x6800 - CS2
//0x6C00 - CS3
//0x7000 - CS4
//0x7400 - CS5
//0x7800 - CS6
//0x7C00 - CS7

//0x7C00 - 373
//#define PORT (*(volatile uint8_t*)0x7C00)
//#define BTNS (*(volatile uint8_t*)0x6600)
//#define FLAGS (*(volatile uint8_t*)0x6200)

typedef enum {SHOW_TIME, SHOW_SECONDS, SHOW_DATE, SHOW_YEAR} state_t;
state_t state = SHOW_TIME; 

const uint8_t spo_msg[11] = {pHH1, pEH, pLL, pOW, pPA5, pWW, pOR, pLL, pDD1, pPA5, pSTOP}; // Hello World

uint32_t last_state_change = 0;
uint32_t last_uptime = 0;
uint8_t last_millis = 0;

char buffer[64];

key_t key0, key1;

char* __fastcall__ utoa (unsigned val, char* buf, int radix);
size_t __fastcall__ strlen (const char* s);

void update_display (void);
void key0_func (void);
void key1_func (void);


int main (void) {

	port_write(0x80);
	
    mc6840_init();
    m6242_init();
    mos6551_init();
	
	CLI();
	
	key_init(&key0, BTN0, key0_func);
	key_init(&key1, BTN1, key1_func);
    
    spo256_play(spo_msg);
	mos6551_puts("Start!\r\n");
	
	while(1) {
		
		if (uptime() != last_uptime) {					// New tick from interrupt
			last_uptime = uptime();						// Clear it first
			update_display();       
		}
        
        if ( (uint8_t) (millis() - last_millis) >= 10) {       //10x25ms
			last_millis = millis();
			port_tgl(0x85);									//Toggle both LEDs and watchgod line
		}
		
		key_update(&key0);
		key_update(&key1);
		mos6551_handle_rx();        
        spo256_handle();
	}
	
	return 0;
}


void update_display (void) {
	static uint8_t dot = 0;
	
	switch(state) {
		case SHOW_TIME:
		m6242_read_time_raw(buffer);
		disp_write(buffer, (dot ? DOT1 : 0x00) );
		dot = !dot;
		break;
		
		case SHOW_SECONDS:
		m6242_read_time_raw(buffer);
		buffer[0] = 16;		// This digit is empty
		buffer[1] = 16;		// This digit is empty
		buffer[2] = buffer[4];
		buffer[3] = buffer[5];
		disp_write(buffer, DOT1);
		if ( (uint32_t) (uptime() - last_state_change) >= 60 ) { state = SHOW_TIME; }
		break;
		
		case SHOW_DATE:
		m6242_read_date_raw(buffer);
		disp_write(buffer, DOT1);
		if ( (uint32_t) (uptime() - last_state_change) >= 60 ) { state = SHOW_TIME; }
		break;
		
		case SHOW_YEAR:
		m6242_read_date_raw(buffer);
		buffer[0] = 2;
		buffer[1] = 0;
		buffer[2] = buffer[4];
		buffer[3] = buffer[5];
		disp_write(buffer, DOT3);
		if ( (uint32_t) (uptime() - last_state_change) >= 60 ) { state = SHOW_TIME; }
		break;
	}    	
}


void key0_func (void) {
	state++;
	if (state > SHOW_YEAR) state = SHOW_TIME;
	last_state_change = uptime();
	update_display();
}

void key1_func (void) {
	spo256_play(spo_msg);
}
