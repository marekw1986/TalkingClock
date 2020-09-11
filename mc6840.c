#include <6502.h>
#include "mc6840.h"

volatile uint8_t milliseconds = 0;
volatile uint32_t uptime_value = 0;
volatile uint8_t dcf_data[8];
volatile uint8_t dcf_count = 0;
volatile uint8_t dcf_work_buffer[8];
volatile uint8_t dcf_frame_received = 0;

uint32_t tmp;

void* __fastcall__ memcpy (void* dest, const void* src, size_t count);


void __fastcall__ mc6840_init (void) {
    MC6840_CON13 = TM_COUNTER_OUTPUT_ENABLE  | TM_INTERUPT_DISABLE | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_SYS_CLK | TMCR3_T3_CLK_NO_PRESCALER;	//CON3 first by default after reset. TIMER3 generates sound. Output enable and sys clk
    MC6840_CON2  = TM_COUNTER_OUTPUT_ENABLE  | TM_INTERUPT_ENABLE  | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_SYS_CLK | TMCR2_WRITE_CR1;				//CON2 accessed directly..TIMER2 is used for systick and time base for DCF77. Output enabled and sys clk.
    MC6840_CON13 = TM_COUNTER_OUTPUT_DISABLE | TM_INTERUPT_ENABLE | TM_PULSE_WIDTH_COMP_MODE1 | TM_NORMAL_16BIT | TM_EXT_CLK | TMCR1_ALL_TIMERS_ALLOWED;	//CON1. TIMER1 to measure DCF77 pulses length, so external source and interrupt enabled.
	//Remember about endianess - MC6800 family is big endian, 6502 is little endian. Remember that timer is decremented.
	MC6840_TIMER1 = Swap2Bytes(0xFFFF); 
    MC6840_TIMER2 = Swap2Bytes(0x9E57);       //25ms interrupt (0xFFFF - 25000) - it is decremented!
    MC6840_TIMER3 = Swap2Bytes(0xF82F);       //500 Hz signal on audio output
}

uint8_t __fastcall__ millis(void) {;
    return milliseconds;
}

uint32_t __fastcall__ uptime (void) {
	SEI();
	tmp = uptime_value;
	CLI();
	return tmp;
}

void __fastcall__ set_sound_frequency (uint16_t freq) {
    if (freq < 24) return;          //Min required frequency. It will solve div/0 and word size issues.
    MC6840_TIMER2 = Swap2Bytes(0xFFFF-(1000000/freq));
}

void __fastcall__ dcf_analyze (uint16_t pulse_len) {
	uint8_t tmp, tmp2;
	
	pulse_len = 0xFFFF - pulse_len;						//Timer counts in reverse, so we need to convert
	
	tmp=dcf_count/8; 
	tmp2=dcf_count%8;
	
	if (pulse_len < 3) {								//Pulse shorter than 75ms (3*25ms) - invalid
		dcf_count = 0;									//Clear bit count
	}
	else if (pulse_len >= 3 && pulse_len <= 5) {		//Valid bit 0 75-125 ms (100 ms)
		dcf_data[tmp] = dcf_data[tmp] & (~(1<<tmp2)); 	//writnig 0
		dcf_count++; 									//next bit	
	}
	else if (pulse_len >= 7 && pulse_len <= 9) {		//Valid bit 1 175-225 ms (200 ms)
		dcf_data[tmp] = dcf_data[tmp] | (1<<tmp2); 		//writnig 1
		dcf_count++; 									//next bit			
	}
	else if (pulse_len > 39 && pulse_len < 84) {		//Valid synchro null 59bit (975-2100 ms)
		dcf_count = 0;
	}
	
	if (dcf_count > 58) {								//End of receiving, now validate data
		dcf_count = 0;									//Prevent buffer overflow
		memcpy(dcf_work_buffer, dcf_data, 8);			//Copy received data to work buffer, it will be processed in main loop
		dcf_frame_received = 1;
	}
}

void __fastcall__ dcf_handle (void) {
	if (dcf_frame_received) {
		dcf_frame_received = 0;
		//Process received frame here!
	}
}
