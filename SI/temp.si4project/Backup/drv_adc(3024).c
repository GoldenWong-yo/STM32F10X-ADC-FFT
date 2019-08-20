#include "drv_adc.h"
#include "global.h"




#define TIM2_PERIOD 1953
#define TIM3_PERIOD 1953



u16 ADC_SourceData[SAMPLS_NUM][ADC_CHANNEL_NUMS] = {0};





void ADC_TIM2_GPIO_Configuration(void)        //ADC���ú���
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIOAʱ��

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

	//����tim2����Ƶ��Ϊ512Hz
	TIM_TimeBaseStructure.TIM_Period = TIM2_PERIOD - 1;				//����2msһ��TIM2�Ƚϵ�����
	TIM_TimeBaseStructure.TIM_Prescaler = 71;						//ϵͳ��Ƶ72M�������Ƶ71���൱��1000K�Ķ�ʱ��2ʱ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = (TIM2_PERIOD - 1) / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//�����PWM1ҪΪLow��PWM2��ΪHigh
	TIM_OC3Init(TIM2, & TIM_OCInitStructure);
	
	//TIM_InternalClockConfig(TIM2);
	
	//TIM_OC3PreloadConfig(TIM2,TIM_OCPreload_Enable);
	
	//TIM_UpdateDisableConfig(TIM2,DISABLE);

	//TIM_Cmd(TIM2, ENABLE);//�����򿪶�ʱ��ʹ��

	//TIM_CtrlPWMOutputs(TIM2,ENABLE);
}

void ADC_TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseInitStructure.TIM_Period = TIM3_PERIOD - 1;				//����512Hz����Ƶ��
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//ʱ�ӷ�Ƶ��TIM3��ͨ�ö�ʱ����������ʱ����������
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//����ɨ��
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);				//ѡ��TRGO��Ϊ����ԴΪ��ʱ������ʱ��
	
	TIM_Cmd(TIM3,ENABLE);												//������ʱ��3
}

void ADC_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				//ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;		//�ܽ�1/4/5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//ģ������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//GPIO��
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

	DMA_Cmd(DMA1_Channel1, ENABLE);//ʹ��DMA	

	ADC_DMA_NVIC_Configuration();
}

void ADC_Init_Configuration(void)//ADC���ú���
{
	ADC_InitTypeDef  ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode       = ENABLE;								//����ǵ�ͨ�����ر�ɨ��ģʽ
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
*adc��ʼ������
*1.Ŀ     �ģ���ADC����ֵ����fft���㣬��ȡ��������������ԣ�������Ҫ����50Hz��ѡ��������/�ʹ�г����
*2.��     �����ƻ�ADC����Ƶ��Ϊ512Hz������1024�������fft���㣬ʹƵ��ֱ��ʴﵽ0.5Hz
*3.��     �ã�����ADC���������õ�512Hz������tim3��ʱ��������
*4.��     ������ϵͳʱ��Ϊ72MHzʱ��ADCʱ��������9MHz�������������ԼΪ256��ʱ�����ڣ�����ADC�������Ƶ������ԼΪ35156.25Hz����������Ҫ��512Hz��
*            ΪʲôҪѡ512Hz�����ڲ��ñ�׼fft��4�㷨����������������4�ı�������Ϊ��ʹƵ��ֱ���Ϊʶ�𷽱㣨0.5Hz������Ϊ�������������
*            �����ƻ�����5120���㣨ʹƵ��ֱ��ʴﵽ0.1Hz�������ǵ�Ƭ��RAM�ռ䲻�����������Ŀ��Գ���
*5.���֪ʶ��������������Ƶ������ڱ���������Ƶ�ʵ�����
*            Ƶ��ֱ��� = fs/N����fs������Ƶ�ʣ�N������������
*            ��Ƶ�������У�FFT_Mag[i]�������˼��Ƶ�ʷ���Ϊi*(fs/N)�ķ�ֵΪFFT_Mag[i]
************************************************************************************************************************************************/
void Adc_Init(void)
{
	ADC_GPIO_Configuration();
	
	//ADC_TIM2_Configuration();

	ADC_TIM3_Configuration();

	ADC_DMA_Configuration();

	ADC_Init_Configuration();
}

//ADC_DMA�жϷ������
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		global.adc_finish_fg = true;
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}




