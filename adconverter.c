#include <LPC21xx.H>
#include "adconverter.h"

#define mPIN_FUNCTION_AIN0 (1<<22)
#define mPIN_SELECT_AIN0 (1<<0)
#define mAD_CONVERTER_ENABLE (1<<21)
#define mAD_CONVERTER_BURST (1<<16)
#define mAD_CONVERTER_DONE (1<<31)
#define mAD_CONVERTER_RESULT 0x0000FFC0
#define AD_CONVERTER_RESULT_POSITION 6
#define AD_CONVERTER_DONE_POSITION 31

void ADConverterInit() {
	PINSEL1 |= mPIN_FUNCTION_AIN0;
	ADCR |= mPIN_SELECT_AIN0 | mAD_CONVERTER_ENABLE | mAD_CONVERTER_BURST;
}

unsigned int ADConverter_GetResult() {
	return ((ADDR & mAD_CONVERTER_RESULT) >> AD_CONVERTER_RESULT_POSITION);
}

unsigned char fADConverter_IfComplete() {
	return ((ADDR & mAD_CONVERTER_DONE) >> AD_CONVERTER_DONE_POSITION);
}
