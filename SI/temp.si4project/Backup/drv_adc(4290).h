#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_

#include "stm32f10x.h"

#define SINGLECHANNEL 1

#define ADC_CHANNEL_NUMS 	3
#define SAMPLS_NUM        	1024

typedef enum{
	FFT_CHANNEL_1 = 0,
	FFT_CHANNEL_2,
	FFT_CHANNEL_3,
}EN_FFT_CHANNEL;


#ifdef __cplusplus
extern "C" {
#endif


#if SINGLECHANNEL
extern u16 ADC_SourceData[SAMPLS_NUM];
#else
extern u16 ADC_SourceData[SAMPLS_NUM][ADC_CHANNEL_NUMS];
#endif

void Adc_Init(void);


#ifdef __cplusplus
}
#endif

#endif


