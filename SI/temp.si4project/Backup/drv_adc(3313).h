#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_

#include "stm32f10x.h"


#define ADC_CHANNEL_NUMS 	3
#define SAMPLS_NUM        	1024



#ifdef __cplusplus
extern "C" {
#endif



extern u16 ADC_SourceData[SAMPLS_NUM];


void Adc_Init(void);


#ifdef __cplusplus
}
#endif

#endif


