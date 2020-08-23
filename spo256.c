#include "spo256.h"

#define SPO256 (*(uint8_t*)0x6300) 
#define SPO256_STATUS (*(uint8_t*)0x6200)
#define SPO_LRQ 0x08
#define SPO_SBY 0x10

uint8_t * spo256_pointer = 0;

void spo256_handle (void) {
    if (!spo256_pointer) return;                        // Pointer null, nothing to do - return.
    if (SPO256_STATUS & SPO_LRQ) return;             	// SP0256 buffer is full - return
    SPO256 = *spo256_pointer;                           // Load next phoneme byte to SP0256
    spo256_pointer++;
    if (*spo256_pointer == pSTOP) spo256_pointer = 0;     // Last phoneme - pointer is NULL now
}

void spo256_play (const uint8_t * pointer) {
    spo256_pointer = (uint8_t*)pointer;
}

void spo256_stop (void) {
    spo256_pointer = 0;
}
