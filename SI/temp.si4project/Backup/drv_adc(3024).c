#include "drv_adc.h"
#include "global.h"




#define TIM2_PERIOD 1953
#define TIM3_PERIOD 1953



u16 ADC_SourceData[SAMPLS_NUM][ADC_CHANNEL_NUMS] = {0};





void ADC_TIM2_GPIO_Configuration(void)        //ADC配置函数
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				//TIM2_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_TIM2_Configuration(void)
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	ADC_TIM2_GPIO_Configuration();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_DeInit(TIM2);

	//配置tim2触发频率为512Hz
	TIM_TimeBaseStructure.TIM_Period = TIM2_PERIOD - 1;				//设置2ms一次TIM2比较的周期
	TIM_TimeBaseStructure.TIM_Prescaler = 71;						//系统主频72M，这里分频71，相当于1000K的定时器2时钟
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = (TIM2_PERIOD - 1) / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//如果是PWM1要为Low，PWM2则为High
	TIM_OC3Init(TIM2, & TIM_OCInitStructure);
	
	//TIM_InternalClockConfig(TIM2);
	
	//TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	
	//TIM_UpdateDisableConfig(TIM2,DISABLE);

	//TIM_Cmd(TIM2, ENABLE);//最后面打开定时器使能

	//TIM_CtrlPWMOutputs(TIM2,ENABLE);
}

void ADC_TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = TIM3_PERIOD - 1;				//设置512Hz采样频率
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//时钟分频，TIM3是通用定时器，基本定时器不用设置
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上扫描
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);				//选择TRGO作为触发源为定时器更新时间
	
	TIM_Cmd(TIM3,ENABLE);												//开启定时器3
}

void ADC_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				//使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;		//管脚1/4/5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//模拟输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//GPIO组
}

void ADC_DMA_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel					 = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_ClearITPendingBit(DMA1_IT_TC1);

	DMA_ITConfig(DMA1_Channel1,DMA1_IT_TC1,ENABLE);
}

void ADC_DMA_Configuration(void)
{
	DMA_InitTypeDef  DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)ADC_SourceData;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize         = ADC_CHANNEL_NUMS*SAMPLS_NUM;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	DMA_Cmd(DMA1_Channel1, ENABLE);//使能DMA	

	ADC_DMA_NVIC_Configuration();
}

void ADC_Init_Configuration(void)//ADC配置函数
{
	ADC_InitTypeDef  ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode       = ENABLE;								//如果是单通道，关闭扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel       = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//==========================================================================   
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  1, ADC_SampleTime_239Cycles5);	//AI_VS_A1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  2, ADC_SampleTime_239Cycles5);	//AI_VS_B1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  3, ADC_SampleTime_239Cycles5);	//AI_VS_C1

	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



/***********************************************************************************************************************************************
*adc初始化配置
*1.目     的：用ADC采样值进行fft运算，求取采样后波形相关属性（本例主要分析50Hz正选波，及高/低次谐波）
*2.方     案：计划ADC采样频率为512Hz，采样1024个点进行fft运算，使频域分辨率达到0.5Hz
*3.配     置：由于ADC本身不能配置到512Hz，采用tim3定时触发采样
*4.其     他：当系统时钟为72MHz时，ADC时钟最慢是9MHz，采样周期最大约为256个时钟周期，所以ADC本身采样频率最慢约为35156.25Hz（大于我需要的512Hz）
*            为什么要选512Hz：由于采用标准fft基4算法（采样点数必须是4的倍数），为了使频域分辨率为识别方便（0.5Hz），又为了满足采样定理
*            本来计划采样5120个点（使频域分辨率达到0.1Hz），但是单片机RAM空间不够，有条件的可以尝试
*5.相关知识：采样定理：采样频率需大于被采样波形频率的两倍
*            频域分辨率 = fs/N，（fs：采样频率，N：采样点数）
*            幅频特性序列：FFT_Mag[i]代表的意思，频率分量为i*(fs/N)的幅值为FFT_Mag[i]
************************************************************************************************************************************************/
void Adc_Init(void)
{
	ADC_GPIO_Configuration();
	
	//ADC_TIM2_Configuration();

	ADC_TIM3_Configuration();

	ADC_DMA_Configuration();

	ADC_Init_Configuration();
}

//ADC_DMA中断服务程序
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		global.adc_finish_fg = true;
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}




