#include "spo256.h"

#define SPO256 (*(uint8_t*)0x0380)          //TODO
#define SPO256_STATUS (*(uint8_t*)0x0381)    //TODO

uint8_t spo256_buffer[256];
uint16_t spo256_buffer_index = 0;

void spo256_handle (void) {
    if (spo256_buffer[spo256_buffer_index] == 0) return;
    if (!(SPO256_STATUS & 0x04)) return;
    SPO256 = spo256_buffer[spo256_buffer_index];
    spo256_buffer_index++;
}
