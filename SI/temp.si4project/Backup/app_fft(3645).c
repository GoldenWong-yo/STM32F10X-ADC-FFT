#include "app_fft.h"
#include "drv_adc.h"
#include "stm32_dsp.h"
#include "table_fft.h"
#include "math.h"

/****************************************************************************************************
函数名称:InitBufInArray()
函数功能:模拟采样数据，采样数据中包含3种频率正弦波(350Hz，8400Hz，18725Hz)
参数说明:
备     注:在lBufInArray数组中，每个数据的高16位存储采样数据的实部，低16位存储采样数据的虚部(总是为0)
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
函数名称:GetPowerMag()
函数功能:计算各次谐波幅值
参数说明:
备　　注:先将lBufOutArray分解成实部(X)和虚部(Y)，然后计算幅值(sqrt(X*X+Y*Y)
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

