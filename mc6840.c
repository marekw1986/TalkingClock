#include <6502.h>
#include "mc6840.h"

volatile uint8_t milliseconds = 0;
volatile uint32_t uptime_value = 0;

uint32_t tmp;

void mc6840_init (void) {
    MC6840_CON13 = TM_COUNTER_OUTPUT_ENABLE  | TM_INTERUPT_DISABLE | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_SYS_CLK | TMCR3_T3_CLK_NO_PRESCALER;	//CON3 first by default after reset. TIMER3 generates sound. Output enable and sys clk
    MC6840_CON2  = TM_COUNTER_OUTPUT_ENABLE  | TM_INTERUPT_ENABLE  | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_SYS_CLK | TMCR2_WRITE_CR1;				//CON2 accessed directly..TIMER2 is used for systick and time base for DCF77. Output enabled and sys clk.
    MC6840_CON13 = TM_COUNTER_OUTPUT_DISABLE | TM_INTERUPT_DISABLE | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_EXT_CLK | TMCR1_ALL_TIMERS_ALLOWED;	//CON1. TIMER1 to measure DCF77 pulses length, so external source and interrupt enabled.
	//Remember about endianess - MC6800 family is big endian, 6502 is little endian. Remember that timer is decremented.
	MC6840_TIMER1 = Swap2Bytes(0x5544); 
    MC6840_TIMER2 = Swap2Bytes(0x9E57);       //20ms interrupt (0xFFFF - 25000) - it is decremented!
    MC6840_TIMER3 = Swap2Bytes(0xF82F);       //500 Hz signal on audio output
}

uint8_t millis(void) {;
    return milliseconds;
}

uint32_t uptime (void) {
	SEI();
	tmp = uptime_value;
	CLI();
	return tmp;
}

void set_sound_frequency (uint16_t freq) {
    if (freq < 24) return;          //Min required frequency. It will solve div/0 and word size issues.
    MC6840_TIMER2 = Swap2Bytes(0xFFFF-(1000000/freq));
}
