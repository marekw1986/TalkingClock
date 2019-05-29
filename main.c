#include <stdint.h>
#include <stdlib.h>
#include "delay.h"

//0x6000 - CS0
//0x7C00 - 373
#define PORT (*(volatile uint8_t*)0x7C00)

//char* __fastcall__ utoa (unsigned val, char* buf, int radix);
//size_t __fastcall__ strlen (const char* s);


int main (void) {
	
	PORT = 0x84;
	
	while(1) {
		PORT = 0x80;
		delay_ms(250);
		PORT = 0x05;
		delay_ms(250);
	}
	
	return 0;
}
