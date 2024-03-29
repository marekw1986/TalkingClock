#ifndef _MC6840_H_
#define _MC6840_H_

#include <inttypes.h>

#define MC6840_CON13 	(*(volatile uint8_t*)0x6100)
#define MC6840_CON2 	(*(volatile uint8_t*)0x6101)
#define MC6840_STATUS 	(*(volatile uint8_t*)0x6101)
#define MC6840_TIMER1 	(*(volatile uint16_t*)0x6102)
#define MC6840_TIMER2 	(*(volatile uint16_t*)0x6104)
#define MC6840_TIMER3 	(*(volatile uint16_t*)0x6106)

#define Swap2Bytes(val) \
 ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

#define TM_COUNTER_OUTPUT_ENABLE		0x80
#define TM_COUNTER_OUTPUT_DISABLE		0x00

#define TM_INTERUPT_ENABLE				0x40
#define TM_INTERUPT_DISABLE				0x00

#define TM_INT_PULSE_LESS_THAN_TO		0x18
#define TM_INT_PULSE_GREATER_THAN_TO	0x38

#define TM_CONT_OP_MODE1				0x00
#define TM_FREQ_COMP_MODE1				0x08
#define TM_CONT_OP_MODE2				0x10
#define TM_PULSE_WIDTH_COMP_MODE1		0x18
#define TM_SINGLE_SHOT_MODE1			0x20
#define TM_FREQ_COMP_MODE2				0x28
#define TM_SINGLE_SHOT_MODE2			0x30
#define TM_PULSE_WIDTH_COMP_MODE2		0x38

#define TM_NORMAL_16BIT					0x00
#define TM_DUAL_8BIT					0x04

#define TM_EXT_CLK						0x00
#define TM_SYS_CLK						0x02

#define TMCR1_ALL_TIMERS_ALLOWED		0x00
#define TMCR1_ALL_TIMERS_PRESET			0x01

#define TMCR2_WRITE_CR3					0x00
#define TMCR2_WRITE_CR1					0x01

#define TMCR3_T3_CLK_NO_PRESCALER		0x00
#define TMCR3_T3_CLK_DIV8_PRESCALER		0x01


void __fastcall__ mc6840_init (void);
uint8_t __fastcall__ millis(void);
uint32_t __fastcall__ uptime (void);
void __fastcall__ set_sound_frequency (uint16_t freq);
void __fastcall__ dcf_handle (void);

#endif //_MC6840_H_
