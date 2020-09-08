#include "spo256.h"

#define SPO256 (*(uint8_t*)0x6300) 
#define SPO256_STATUS (*(uint8_t*)0x6200)
#define SPO_LRQ 0x08
#define SPO_SBY 0x10

const uint8_t spo_zero[] = {pZZ, pYR, pOW, pSTOP};
const uint8_t spo_one[] = {pWW, pAX, pNN1, pSTOP};
const uint8_t spo_two[] = {pTT2, pUW2, pSTOP};
const uint8_t spo_three[] = {pTH, pRR1, pIY, pSTOP};
const uint8_t spo_four[] = {pFF, pFF, pOR, pSTOP};
const uint8_t spo_five[] = {pFF, pFF, pAY, pVV, pSTOP};
const uint8_t spo_six[] = {pSS, pSS, pIH, pIH, pPA3, pKK2, pSS, pSTOP};
const uint8_t spo_seven[] = {pSS, pSS, pEH, pEH, pVV, pIH, pNN1, pSTOP};
const uint8_t spo_eight[] = {pEY, pPA3, pTT2, pSTOP};
const uint8_t spo_nine[] = {pNN1, pPA1, pAA, pAA, pYY1, pNN1, pSTOP};
const uint8_t spo_ten[] = {pTT2, pEH, pEH, pNN1, pSTOP};
const uint8_t spo_eleven[] = {pIH, pLL, pEH, pEH, pVV, pIH, pNN1, pSTOP};
const uint8_t spo_twelve[] = {pTT2, pWH, pEH, pEH, pLL, pVV, pSTOP};
const uint8_t spo_thiteen[] = {pTH, pER1, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_fourteen[] = {pFF, pOR, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_fifteen[] = {pFF, pIH, pFF, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_sixteen[] = {pSS, pSS, pIH, pPA3, pKK2, pSS, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_seventeen[] = {pSS, pSS, pEH, pVV, pTH, pNN1, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_eighteen[] = {pEY, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_nineteen[] = {pNN1, pAY, pNN1, pPA2, pPA3, pTT2, pIY, pNN1, pSTOP};
const uint8_t spo_twenty[] = {pTT2, pWH, pEH, pEH, pNN1, pPA2, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_thirty[] = {pTH, pER2, pPA2, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_fourty[] = {pFF, pOR, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_fifty[] = {pFF, pFF, pIH, pFF, pFF, pPA2, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_sixty[] = {pSS, pSS, pIH, pPA3, pKK2, pSS, pPA2, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_seventy[] = {pSS, pSS, pEH, pVV, pIH, pNN1, pPA2, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_eighty[] = {pEY, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_ninety[] = {pNN1, pAY, pNN1, pPA3, pTT2, pIY, pSTOP};
const uint8_t spo_hundred[] = {pHH2, pAX, pAX, pNN1, pPA2, pDD2, pRR2, pIH, pIH, pPA1, pDD1, pSTOP};
const uint8_t spo_thousand[] = {pTH, pAA, pAW, pZZ, pTH, pPA1, pPA1, pNN1, pDD1, pSTOP};
const uint8_t spo_million[] = {pMM, pIH, pIH, pLL, pYY1, pAX, pNN1, pSTOP};

const uint8_t spo_first[] = {pFF, pER1, pSS, pPA3, pTT1, pPA3, pSTOP};
const uint8_t spo_third[] = {pTH, pER1, pPA2, pDD1, pPA3, pSTOP};
const uint8_t spo_fourth[] = {pFF, pAO ,pRR2, pTH, pPA3, pSTOP};
const uint8_t spo_fifth[] = {pFF, pIH, pFF, pTH, pPA3, pSTOP};

const uint8_t spo_sunday[] = {pSS, pSS, pAX, pAX, pNN1, pPA2, pDD2, pEY, pSTOP};
const uint8_t spo_monday[] = {pMM, pAX, pAX, pNN1, pPA2, pDD2, pEY, pSTOP};
const uint8_t spo_tuesday[] = {pTT2, pUW2, pZZ, pPA2, pDD2, pEY, pSTOP};
const uint8_t spo_wednesday[] = {pWW, pEH, pEH, pNN1, pZZ, pPA2, pDD2, pEY, pSTOP};
const uint8_t spo_thurdsay[] = {pTH, pER2, pZZ, pPA2, pDD2, pEY, pSTOP};
const uint8_t spo_friday[] = {pFF, pRR2, pAY, pPA2, pDD2, pEY, pSTOP};
const uint8_t spo_saturday[] = {pSS, pSS, pAE, pPA3, pTT2, pPA2, pDD2, pEY, pSTOP};

const uint8_t spo_january[] = {pJH, pAE, pAE, pNN1, pYY2, pXR, pYY1, pSTOP};
const uint8_t spo_february[] = {pFF, pEH, pEH, pPA1, pBB1, pRR2, pUW2, pXR, pIY, pSTOP};
const uint8_t spo_march[] = {pMM, pAR, pPA3, pCH, pSTOP};
const uint8_t spo_april[] = {pEY, pPA3, pPP, pRR2, pIH, pIH, pLL, pSTOP};
const uint8_t spo_may[] = {pMM, pEY, pSTOP};
const uint8_t spo_june[] = {pJH, pUW2, pNN1, pSTOP};
const uint8_t spo_july[] = {pJH, pUW1, pLL, pAY, pSTOP};
const uint8_t spo_august[] = {pAO, pAO, pPA2, pGG2, pAX, pSS, pPA3, pTT1, pSTOP};
const uint8_t spo_september[] = {pSS, pSS, pEH, pPA3, pPP, pPA3, pTT2, pEH, pEH, pPA1, pBB2, pER1, pSTOP};
const uint8_t spo_october[] = {pAA, pPA2, pKK2, pPA3, pTT2, pOW, pPA1, pBB2, pER1, pSTOP};
const uint8_t spo_november[] = {pNN2, pOW, pVV, pEH, pEH, pMM, pPA1, pBB2, pER1, pSTOP};
const uint8_t spo_december[] = {pDD2, pIY, pSS, pSS, pEH, pEH, pMM, pPA1, pBB2, pER1, pSTOP};

const uint8_t spo_hour[] = {pAW, pER1, pSTOP};
const uint8_t spo_minute[] = {pMM, pIH, pNN1, pIH, pPA3, pTT2, pSTOP};
const uint8_t spo_second[] = {pSS, pSS, pEH, pPA3, pKK1, pIH, pNN1, pPA2, pDD1, pSTOP};
const uint8_t spo_of[] = {pAO, pFF, pSTOP};

const uint8_t * spo_numbers[] = {spo_zero, spo_one, spo_two, spo_three, spo_four, spo_five, spo_six, spo_seven, spo_eight, spo_nine, spo_ten, spo_eleven, spo_twelve, spo_thiteen, spo_fourteen, spo_fifteen, spo_sixteen, spo_seventeen, spo_eighteen, spo_nineteen};
const uint8_t * spo_numerals[] = {spo_zero, spo_first, spo_second, spo_third, spo_fourth, spo_fifth};
const uint8_t * spo_tens[] = {spo_zero, spo_ten, spo_twenty, spo_thirty, spo_fourty, spo_fifty, spo_sixty, spo_seventy, spo_eighty, spo_ninety, spo_hundred};
const uint8_t * spo_weekdays[] = {spo_sunday, spo_monday, spo_tuesday, spo_wednesday, spo_thurdsay, spo_friday, spo_saturday};
const uint8_t * spo_months[] = {spo_january, spo_february, spo_march, spo_april, spo_may, spo_june, spo_july, spo_august, spo_september, spo_october, spo_november, spo_december};

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

uint8_t spolen (const char *data) {
	uint8_t i = 0;
	while(*data != pSTOP) {
		data++;
		i++;
	}
	return i;
}
