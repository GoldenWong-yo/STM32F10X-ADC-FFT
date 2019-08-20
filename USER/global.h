#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include "stm32f10x.h"


typedef enum {false = 0, true = !false} bool;

typedef struct{
	bool adc_finish_fg;
}ST_GOLBAL;




extern ST_GOLBAL global;

#endif

