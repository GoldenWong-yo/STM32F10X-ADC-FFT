#include "app_fft.h"
#include "drv_adc.h"
#include "stm32_dsp.h"
#include "table_fft.h"
#include "math.h"

/****************************************************************************************************
��������:InitBufInArray()
��������:ģ��������ݣ����������а���3��Ƶ�����Ҳ�(350Hz��8400Hz��18725Hz)
����˵��:
��     ע:��lBufInArray�����У�ÿ�����ݵĸ�16λ�洢�������ݵ�ʵ������16λ�洢�������ݵ��鲿(����Ϊ0)
****************************************************************************************************/
u32 ADC_FFT_SourceData[SAMPLS_NUM] = {0};
u32 ADC_FFT_OutData[SAMPLS_NUM] = {0};
u32 FFT_Mag[SAMPLS_NUM/2] = {0};

#if 0
u16 lBufInArray[NPT];

void InitBufInArray(void)
{
	unsigned short i;
	float fx;
	for(i=0; i<NPT; i++)
	{
		fx = 1500 * sin(PI2 * i * 55.6 / Fs);
		lBufInArray[i] = ((signed short)fx)<<16;
	}
}
#endif

/******************************************************************************
��������:GetPowerMag()
��������:�������г����ֵ
����˵��:
������ע:�Ƚ�lBufOutArray�ֽ��ʵ��(X)���鲿(Y)��Ȼ������ֵ(sqrt(X*X+Y*Y)
******************************************************************************/
void GetPowerMag(void)
{
	signed short lX,lY;
	float X,Y,Mag;
	unsigned short i;

	#if 1
	for(i=0; i<NPT/2; i++)
	{
		lX  = (ADC_FFT_OutData[i] << 16) >> 16;
		lY  = (ADC_FFT_OutData[i] >> 16);
		
		X = SAMPLS_NUM * ((float)lX) / 32768;
		Y = SAMPLS_NUM * ((float)lY) / 32768;
		
		Mag = sqrt(X * X + Y * Y) / SAMPLS_NUM;
		
		if(i == 0)
			FFT_Mag[i] = (unsigned long)(Mag * 32768);
		else
			FFT_Mag[i] = (unsigned long)(Mag * 65536);
		//printf("%ld\r\n",lBufMagArray[i]);
	}
	//printf("\r\n\r\n");
	#else
	for(i=0; i<NPT/2; i++)
	{
		X  = (ADC_FFT_OutData[i] << 16) >> 16;
		Y  = (ADC_FFT_OutData[i] >> 16);
		FFT_Mag[i] = sqrt(X * X + Y * Y);
	}
	#endif
}

void Get_FFT_Source_Data(void)
{
	u16 i,j;

	//for(i=0; i<ADC_CHANNEL_NUMS; i++)
	{
		for(j=0; j<SAMPLS_NUM; j++)
		{
			ADC_FFT_SourceData[j] = (u32)ADC_SourceData[j];
		}
	}
}

void FFT_test(void)
{
	//InitBufInArray();
	Get_FFT_Source_Data();
	cr4_fft_1024_stm32(ADC_FFT_OutData, ADC_FFT_SourceData, NPT);
	GetPowerMag();
	//while(1);
}

