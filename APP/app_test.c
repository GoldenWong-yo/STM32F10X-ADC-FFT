#include "app_test.h"
#include "stm32f10x.h"
#include "global.h"


void Init_Test_Pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //Ê¹ÄÜGPIOAÊ±ÖÓ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				//TIM2_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Test_Time_Func(void)			//test time
{
	static bool fg = true;

	fg = !fg;
	
	if(fg)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}else{
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	}
}

