#include <6502.h>
#include <stdlib.h>
#include "mc6840.h"
#include "m6242.h"
#include "mos6551.h"
#include "io.h"

volatile uint8_t milliseconds = 0;
volatile uint32_t uptime_value = 0;

volatile uint8_t dcf_data[8];
volatile uint8_t dcf_count = 0;
uint8_t dcf_samples[256];
uint8_t dcf_intervals[256];
uint8_t dcf_samples_head = 0;
uint8_t dcf_samples_tail = 0;
uint8_t dcf_prev = 0;

static void __fastcall__ dcf_analyze_frame (void);
static uint8_t get_dcf_bit(uint8_t n);
static uint8_t dcf_parity(uint8_t x, uint8_t y);
static uint8_t dcf_min (void);
static uint8_t dcf_h (void);
static uint8_t dcf_d (void);
static uint8_t dcf_m (void);
static uint8_t dcf_y (void);


char* __fastcall__ itoa (int val, char* buf, int radix);


char dbuf[32];



void __fastcall__ mc6840_init (void) {
    MC6840_CON13 = TM_COUNTER_OUTPUT_ENABLE  | TM_INTERUPT_DISABLE | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_SYS_CLK | TMCR3_T3_CLK_NO_PRESCALER;	//CON3 first by default after reset. TIMER3 generates sound. Output enable and sys clk
    MC6840_CON2  = TM_COUNTER_OUTPUT_ENABLE  | TM_INTERUPT_ENABLE  | TM_CONT_OP_MODE1 | TM_NORMAL_16BIT | TM_SYS_CLK | TMCR2_WRITE_CR1;				//CON2 accessed directly..TIMER2 is used for systick and time base for DCF77. Output enabled and sys clk.
    MC6840_CON13 = TM_COUNTER_OUTPUT_DISABLE | TM_INTERUPT_ENABLE | TM_INT_PULSE_LESS_THAN_TO | TM_NORMAL_16BIT | TM_EXT_CLK | TMCR1_ALL_TIMERS_ALLOWED;	//CON1. TIMER1 to measure DCF77 pulses length, so external source and interrupt enabled.
	//Remember about endianess - MC6800 family is big endian, 6502 is little endian. Remember that timer is decremented.
	MC6840_TIMER1 = Swap2Bytes(0xFFFF); 
    MC6840_TIMER2 = Swap2Bytes(0x30D4);       //12.5ms interrupt
    MC6840_TIMER3 = Swap2Bytes(0x07D0);       //500 Hz signal on audio output
}

uint8_t __fastcall__ millis(void) {;
    return milliseconds;
}

uint32_t __fastcall__ uptime (void) {
	uint32_t tmp;
	
	SEI();
	tmp = uptime_value;
	CLI();
	return tmp;
}

void __fastcall__ set_sound_frequency (uint16_t freq) {
    if (freq < 16) return;          					//Min required frequency. It will solve div/0 and word size issues.
    MC6840_TIMER2 = Swap2Bytes((uint16_t)(1000000/freq));
}

void __fastcall__ dcf_handle (void) {
	uint8_t tmp, tmp2;
	uint8_t pulse_len;
	uint8_t prev_pulse_interval;
    uint8_t i;
	
	if (dcf_samples_head == dcf_samples_tail) return;
	pulse_len = 0xFF - dcf_samples[dcf_samples_tail];
	prev_pulse_interval = dcf_intervals[dcf_samples_tail];
	dcf_samples_tail++;
	
    //mos6551_puts("Pulse len: ");
	//itoa(pulse_len, dbuf, 10);
	//mos6551_puts(dbuf);
    //mos6551_puts("\r\n");
    //mos6551_puts("Prev pulse interval: ");
    //itoa(prev_pulse_interval, dbuf, 10);
    //mos6551_puts(dbuf);
    //mos6551_puts("\r\n"); 
	
	tmp=dcf_count/8; 
	tmp2=dcf_count%8;
	
	if (prev_pulse_interval > 104) {						//This is a new frame!
        //mos6551_puts("DCF Sync! dcf_count=");
        //mos6551_puts(itoa(dcf_count, dbuf, 10));
        //mos6551_puts("\r\n");
        if (dcf_count == 59) {							//If there is a data
            //mos6551_puts("Frame: ");
            //for (i=0; i<8; i++) {
            //    mos6551_puts(itoa(dcf_data[i], dbuf, 10));
            //    mos6551_putc(' ');
            //}
            //mos6551_puts("\r\n");
            dcf_analyze_frame();						//Analyze old frame
        }		
		dcf_count = 0;
		port_tgl(0x80);
	}
	
	if (pulse_len < 3) {								//Pulse shorter than 75ms (3*25ms) - invalid
		dcf_count = 0;									//Clear bit count
	}
	else if (pulse_len >= 3 && pulse_len <= 5) {		//Valid bit 0 75-125 ms (100 ms)
		dcf_data[tmp] = dcf_data[tmp] & (~(1<<tmp2)); 	//writnig 0
		dcf_count++;									//next bit
		port_tgl(0x04);							
	}
	else if (pulse_len >= 7 && pulse_len <= 9) {		//Valid bit 1 175-225 ms (200 ms)
		dcf_data[tmp] = dcf_data[tmp] | (1<<tmp2); 		//writnig 1
		dcf_count++; 									//next bit		
		port_tgl(0x04);	
	}
	else if (pulse_len > 120) {							//Pulse longer than 3s - error
		dcf_count = 0;
	}
	
	if (dcf_count > 60) {
		dcf_count = 0;									//Prevent buffer overflow

	}
}

void __fastcall__ dcf_analyze_frame (void) {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	
	static uint8_t hour_old = 0;
	static uint8_t day_old = 0;
	static uint8_t month_old = 0;
	static uint8_t year_old = 0;
	
	//Process received frame here!
	if (get_dcf_bit(0) == 0 && get_dcf_bit(20) == 1) { //bit 0 lways == 0, bit 20 always == 1
		if (dcf_parity(21, 27) == get_dcf_bit(28) && dcf_parity(29, 34) == get_dcf_bit(35) && dcf_parity(36, 57) == get_dcf_bit(58)) { //Parity check
			if (get_dcf_bit(17) != get_dcf_bit(18)) { //These bits are never equal
				//Now we need to check if same data received twice in row.
				//Parity check is not good enough
				mos6551_puts("DCF: Parity OK\r\n");
				if (hour_old == dcf_h() && day_old == dcf_d() && month_old == dcf_m() && year_old == dcf_y()) {
					//Values OK
					second = 0;
					minute = dcf_min();
					hour = dcf_h();
					day = dcf_d();
					month = dcf_m();
					year = dcf_y();
					//TODO: Check daylight saving
					//Set time
					if (DCF_ENABLED) {
						mos6551_puts("DCF: Setting time\r\n");
						m6242_settime(hour, minute, second);
						m6242_setdate(day, month, year);
					}
				}
				hour_old = dcf_h();
				day_old = dcf_d();
				month_old = dcf_m();
				year_old = dcf_y();
			}
		}
	}

}


//Get n bit from dcf_data
static uint8_t get_dcf_bit(uint8_t n) {	
	return (dcf_data[n/8]&(1<<(n%8))) ? 1 : 0;
}


static uint8_t dcf_parity(uint8_t x, uint8_t y) {
	uint8_t n,t=0;
	for(n=x;n<=y;n++){
		t=t+get_dcf_bit(n);
	}	
	return t%2; //return bit parity	
}


//Get minutes from received data
static uint8_t dcf_min (void) {
	uint8_t minutes;

	minutes = get_dcf_bit(21);
	minutes += (get_dcf_bit(22)*2);
	minutes += (get_dcf_bit(23)*4);
	minutes += (get_dcf_bit(24)*8);
	minutes += (get_dcf_bit(25)*10);
	minutes += (get_dcf_bit(26)*20);
	minutes += (get_dcf_bit(27)*40);
	return minutes;
}

//Get hour from received data
static uint8_t dcf_h (void) {
	uint8_t hours;

	hours = get_dcf_bit(29);
	hours += (get_dcf_bit(30)*2);
	hours += (get_dcf_bit(31)*4);
	hours += (get_dcf_bit(32)*8);
	hours += (get_dcf_bit(33)*10);
	hours += (get_dcf_bit(34)*20);
	return hours;	
}	


//Get day from received data
static uint8_t dcf_d (void) {
	uint8_t days;

	days = get_dcf_bit(36);
	days += (get_dcf_bit(37)*2);
	days += (get_dcf_bit(38)*4);
	days += (get_dcf_bit(39)*8);
	days += (get_dcf_bit(40)*10);
	days += (get_dcf_bit(41)*20);
	//day of week
	// ... 42-44 1-pon 7-niedz
	return days;
}


//Get month from received data
static uint8_t dcf_m (void) {
	uint8_t months;	

	months = get_dcf_bit(45);
	months += (get_dcf_bit(46)*2);
	months += (get_dcf_bit(47)*4);
	months += (get_dcf_bit(48)*8);
	months += (get_dcf_bit(49)*10);
	return months;
}


//Get year from received data
static uint8_t dcf_y (void){ 
	uint8_t years;
	
	years = get_dcf_bit(50);
	years += (get_dcf_bit(51)*2);
	years += (get_dcf_bit(52)*4);
	years += (get_dcf_bit(53)*8);
	years += (get_dcf_bit(54)*10);
	years += (get_dcf_bit(55)*20);
	years += (get_dcf_bit(56)*40);
	years += (get_dcf_bit(57)*80);
	return years;	
}
