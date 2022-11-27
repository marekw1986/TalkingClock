#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

#define PORT (*(volatile uint8_t*)0x7C00)
#define BTNS (*(volatile uint8_t*)0x6600)

#define DIG0 			  (*(volatile uint8_t*)0x6603)
#define DIG1 			  (*(volatile uint8_t*)0x6602)
#define DIG2 			  (*(volatile uint8_t*)0x6601)
#define DIG3 			  (*(volatile uint8_t*)0x6600)

#define BTN0	0x10
#define BTN1	0x20
#define BTN2	0x40
#define BTN3	0x80

#define SND_ENABLE_PIN	0x40

#define DOT_SEGMENT 0x04

#define DOT0	0x01
#define DOT1	0x02
#define DOT2	0x04
#define DOT3	0x08

#define SHORT_WAIT 6    // 3 * 12.5ms = 75 ms.
#define LONG_WAIT 120    // 120 * 12.5ms = 1500 ms.

#define SND_START	port_clr(0x40)
#define SND_STOP	port_set(0x40)

typedef struct key {
    uint8_t pin;
    uint8_t last_state;
	uint8_t timer;
	void (*push_proc)(void);
} key_t;

void __fastcall__ key_init (key_t *key, uint8_t pin, void (*push_proc)(void));
void __fastcall__ key_update (key_t *key);
void __fastcall__ port_write (uint8_t data);
void __fastcall__ port_set (uint8_t data);
void __fastcall__ port_clr (uint8_t data);
void __fastcall__ port_tgl (uint8_t data);
void __fastcall__ disp_write (uint8_t* data, uint8_t dp);
void __fastcall__ feed_hungry_watchdog (void);

#endif
