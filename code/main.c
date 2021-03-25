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
#define PUNS (uint16_t*)0x2000

typedef enum {SHOW_TIME, SHOW_SECONDS, SHOW_DATE, SHOW_YEAR} state_t;
state_t state = SHOW_TIME; 

const uint8_t spo_msg[12] = {pHH1, pEH, pLL, pOW, pPA5, pWW, pER1, pLL, pPA2, pDD1, pPA3, pSTOP}; // Hello World

uint8_t spo_buff[256];

uint32_t last_state_change = 0;
uint32_t last_uptime = 0;
uint8_t last_millis = 0;

char buffer[64];
static uint16_t *punptr;
static uint8_t *ptr;
static uint8_t len, tmp;

key_t key0, key1, key2, key3;

char* __fastcall__ utoa (unsigned val, char* buf, int radix);
void* __fastcall__ memcpy (void* dest, const void* src, size_t count);
size_t __fastcall__ strlen (const char* s);

void update_display (void);
void key0_func (void);
void key1_func (void);
void key2_func (void);
void key3_func (void);


int main (void) {

	port_write(0xC1);
	
	punptr = PUNS;
	
    mc6840_init();
    m6242_init();
    mos6551_init();
	
	CLI();
	
	key_init(&key0, BTN0, key0_func);
	key_init(&key1, BTN1, key1_func);
	key_init(&key2, BTN2, key2_func);
	key_init(&key3, BTN3, key3_func);
    
    spo256_play(spo_msg);
	mos6551_puts("Start!\r\n");
	
	while(1) {
		
		if (uptime() != last_uptime) {					// New tick from interrupt
			last_uptime = uptime();						// Clear it first
			update_display();       
		}
        
        if ( (uint8_t) (millis() - last_millis) >= 10) {       //10x25ms
			last_millis = millis();
			//port_tgl(0x84);									//Toggle both LEDs
			feed_hungry_watchdog();							//Reset watchdog
		}
		
		key_update(&key0);
		key_update(&key1);
		key_update(&key2);
		key_update(&key3);
		mos6551_handle_rx();        
        spo256_handle();
        dcf_handle();
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
	spo256_play((uint8_t*)*punptr);
	punptr++;
	if (*punptr == 0x00) punptr = PUNS;
	//spo256_play(spo_msg);
}

void key2_func (void) {
    ptr = spo_buff;
    
	//READ TIME
	m6242_read_time_raw(buffer);
    
    //Prepare hour phonemes
	tmp = (buffer[0] * 10) + buffer[1];
	if (tmp < 20) {
		memcpy(ptr, spo_numbers[tmp], len = spolen(spo_numbers[tmp]));
		ptr += len;
	}
	else {
        if (buffer[0] > 0) {
			memcpy(ptr, spo_tens[buffer[0]], len = spolen(spo_tens[buffer[0]]));
			ptr += len;
		}
        if (buffer[1] > 0) { 
			memcpy(ptr, spo_numbers[buffer[1]], len = spolen(spo_numbers[buffer[1]]));
			ptr += len;
		}		
	}
    
    *ptr = pPA3;
    ptr++;
	memcpy(ptr, spo_hour, len = spolen(spo_hour));
	ptr += len;
    if (tmp != 1) {
        *ptr = pZZ;
        ptr++;
    }    
    *ptr = pPA3;
    ptr++;	    
    
    //Prepare minutes phonemes
    tmp = (buffer[2] * 10) + buffer[3];
    if (tmp < 20) {
		memcpy(ptr, spo_numbers[tmp], len = spolen(spo_numbers[tmp]));
		ptr += len;
    }
    else {
        if (buffer[2] > 0) {
			memcpy(ptr, spo_tens[buffer[2]], len = spolen(spo_tens[buffer[2]]));
			ptr += len;
		}
        if (buffer[3] > 0) { 
			memcpy(ptr, spo_numbers[buffer[3]], len = spolen(spo_numbers[buffer[3]]));
			ptr += len;
		}
    }
    
    //Prepare seconds phonemes
    *ptr = pPA3;
    ptr++;
	memcpy(ptr, spo_minute, len = spolen(spo_minute));
	ptr += len;
    if (tmp != 1) {
        *ptr = pZZ;
        ptr++;
    }    
    *ptr = pPA3;
    ptr++;	        
    
    tmp = (buffer[4] * 10) + buffer[5];
    if (tmp < 20) {
        memcpy(ptr, spo_numbers[tmp], len = spolen(spo_numbers[tmp]));
        ptr += len;
    }
    else {
        if (buffer[4] > 0) { 
			memcpy(ptr, spo_tens[buffer[4]], len = spolen(spo_tens[buffer[4]]));
			ptr += len;
		}
        if (buffer[5] > 0) { 
			memcpy(ptr, spo_numbers[buffer[5]], len = spolen(spo_numbers[buffer[5]]));
			ptr += len;
		}
    }
    
    *ptr = pPA5;
    ptr++;
	memcpy(ptr, spo_second, len = spolen(spo_second));
	ptr += len;
    if (tmp != 1) {
        *ptr = pZZ;
        ptr++;
    }
    *ptr = pPA3;
    ptr++;	      
    *ptr = pSTOP;	
	spo256_play(spo_buff);
}


void key3_func (void) {
	ptr = spo_buff;
	//READ DATE
	m6242_read_date_raw(buffer);

    //Prepare fay of the week phonemes
    /*
    memcpy(ptr, spo_weekdays[buffer[6]], len = spolen(spo_weekdays[buffer[6]]));
    ptr += len;
    *ptr = pPA5;
    ptr++;
    */
    
    //Prepare month phonemes
    tmp = (buffer[2] * 10) + buffer[3];
    tmp--;
    memcpy(ptr, spo_months[tmp], len = spolen(spo_months[tmp]));
    ptr += len;
    *ptr = pPA5;
    ptr++;

    //Prepare day of month phonemes
	tmp = (buffer[0] * 10) + buffer[1];
	if (tmp < 6) {
		memcpy(ptr, spo_numerals[tmp], len = spolen(spo_numerals[tmp]));
		ptr += len;
	}
	else if (tmp < 20) {
		memcpy(ptr, spo_numbers[tmp], len = spolen(spo_numbers[tmp]));
		ptr += len;
		*ptr = pTH;
		ptr++;
	}
	else {
        if (buffer[0] > 0) {
			memcpy(ptr, spo_tens[buffer[0]], len = spolen(spo_tens[buffer[0]]));
			ptr += len;
		}
        if (buffer[1] > 0) { 
			if (buffer[1] < 6) {
				memcpy(ptr, spo_numerals[buffer[1]], len = spolen(spo_numerals[buffer[1]]));
				ptr += len;
			}
			else {
				memcpy(ptr, spo_numbers[buffer[1]], len = spolen(spo_numbers[buffer[1]]));
				ptr += len;
				*ptr = pTH;
				ptr++;								
			}
		}		
	}
	
    *ptr = pPA5;
    ptr++;	
    
    //Prepare year phonemes      
    //We assume the years is > 2000
    memcpy(ptr, spo_numbers[2], len = spolen(spo_numbers[2]));      //We need "two" to be said first
    ptr += len;
    memcpy(ptr, spo_thousand, len = spolen(spo_thousand));          //Then "thousand" needs to be said
    ptr += len;
    *ptr = pPA3;
    ptr++;
    //Phonemes for last two digits of the year
    tmp = (buffer[4] * 10) + buffer[5];
    if (tmp < 20) {
        memcpy(ptr, spo_numbers[tmp], len = spolen(spo_numbers[tmp]));
        ptr += len;
    }
    else {
        if (buffer[4] > 0) { 
			memcpy(ptr, spo_tens[buffer[4]], len = spolen(spo_tens[buffer[4]]));
			ptr += len;
		}
        if (buffer[5] > 0) { 
			memcpy(ptr, spo_numbers[buffer[5]], len = spolen(spo_numbers[buffer[5]]));
			ptr += len;
		}
    }
    
    *ptr = pPA5;
    ptr++;	      
    *ptr = pSTOP;    
	spo256_play(spo_buff);
}
