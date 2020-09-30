#include "io.h"
#include "mc6840.h"

static uint8_t port_cache = 0;

const uint8_t digits[17] = {0xFA, 0x30, 0xD9, 0x79, 0x33, 0x6B, 0xEB, 0x38, 0xFB, 0x7B, 0xBB, 0xE3, 0xC1, 0xF1, 0xCB, 0x8B, 0x00};

void __fastcall__ key_init (key_t *key, uint8_t pin, void (*push_proc)(void)) {
	key->pin = pin;
	key->timer = 0;
	key->last_state = 0;
	key->push_proc = push_proc;
}


void __fastcall__ key_update (key_t *key) {
	static uint8_t key_press;
	
    key_press = !(BTNS & key->pin);
    if (key_press != (key->last_state)) {
		if (key_press) {
			key->timer = millis();
		}
		else {
			if ( key->timer && ( (uint8_t)(millis()-(key->timer)) > SHORT_WAIT ) ) {
				if (key->push_proc) (key->push_proc)();
				key->timer = 0;
			}
		}
		key->last_state = key_press;
	}
}


void __fastcall__ port_write (uint8_t data) {
    port_cache = data;
    PORT = port_cache;
}


void __fastcall__ port_set (uint8_t data) {
    port_cache |= data;
    PORT = port_cache;
}


void __fastcall__ port_clr (uint8_t data) {
    port_cache &= ~data;
    PORT = port_cache;
}


void __fastcall__ port_tgl (uint8_t data) {
    port_cache ^= data;
    PORT = port_cache;
}


void disp_write (uint8_t* data, uint8_t dp){
    DIG0 = (digits[data[0]]) | ((dp & DOT0) ? DOT_SEGMENT : 0x00);      
    DIG1 = (digits[data[1]]) | ((dp & DOT1) ? DOT_SEGMENT : 0x00);     
    DIG2 = (digits[data[2]]) | ((dp & DOT2) ? DOT_SEGMENT : 0x00);   
    DIG3 = (digits[data[3]]) | ((dp & DOT3) ? DOT_SEGMENT : 0x00);  
}
