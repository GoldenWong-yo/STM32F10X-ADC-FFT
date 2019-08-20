#include "app_fft.h"
#include "drv_adc.h"
#include "stm32_dsp.h"
#include "table_fft.h"
#include "math.h"






u32 FFT_SourceData[SAMPLS_NUM] = {0};	//fft��������
u32 FFT_OutData[SAMPLS_NUM] = {0};		//fft�������
u32 FFT_Mag[SAMPLS_NUM/2] = {0};		//��Ƶ�������У���Ŵ���Ƶ�ʷ�����ֵ�����ֵ��С������FFT��Ƶ�׽���ǹ����ο�˹��Ƶ�ʶԳƵģ�����ֻ����һ��ĵ㼴�ɣ�


#if 0
/****************************************************************************************************
��������:InitBufInArray()
��������:ģ��������ݣ����������а���3��Ƶ�����Ҳ�(350Hz��8400Hz��18725Hz)
����˵��:
��     ע:��lBufInArray�����У�ÿ�����ݵĸ�16λ�洢�������ݵ�ʵ������16λ�洢�������ݵ��鲿(����Ϊ0)
****************************************************************************************************/

u16 lBufInArray[SAMPLS_NUM];

void InitBufInArray(void)
{
	unsigned short i;
	float fx;
	for(i=0; i<SAMPLS_NUM; i++)
	{
		fx = 1500 * sin(PI2 * i * 55.6 / Fs);
		lBufInArray[i] = ((signed short)fx)<<16;
	}
}
#endif

/********************************************************************************************************
��������:GetPowerMag()
��������:�������г����ֵ
����˵��:
������ע:�Ƚ�ADC_FFT_OutData�ֽ��ʵ��(X)���鲿(Y)��Ȼ������Ƶ��������FFT_Mag
         �������ο���ҳ��https://wenku.baidu.com/view/08ccee0984868762cbaed532.html�����ڷ�Ƶ���Լ��㲿��
**********************************************************************************************************/
void GetPowerMag(void)
{
	signed short lX,lY;
	float X,Y,Mag;
	unsigned short i;

	#if 1
	for(i=0; i<SAMPLS_NUM/2; i++)
	{
		lX = (FFT_OutData[i] << 16) >> 16;
		lY = (FFT_OutData[i] >> 16);
		
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
	for(i=0; i<SAMPLS_NUM/2; i++)
	{
		X = (FFT_OutData[i] << 16) >> 16;
		Y = (FFT_OutData[i] >> 16);
		FFT_Mag[i] = sqrt(X * X + Y * Y);
	}
	#endif
}

/************************************************
*������fft��������в�����Դ���ݸ���
*��Դ���ݿ�������һ���ڴ�
*�˲�����Ա��滻Ϊ������fft����ʱ���ر�adcת��
*************************************************/
void Get_FFT_Source_Data(EN_FFT_CHANNEL channel_idx)
{
	u16 i,j;

	for(i=0; i<SAMPLS_NUM; i++)
	{
		#if SINGLECHANNEL
		FFT_SourceData[i] = (u32)ADC_SourceData[i];
		#else
		FFT_SourceData[i] = (u32)ADC_SourceData[i][channel_idx];
		#endif
	}
}

void FFT_test(void)
{
	//InitBufInArray();
	Get_FFT_Source_Data(FFT_CHANNEL_1);
	cr4_fft_1024_stm32(FFT_OutData, FFT_SourceData, SAMPLS_NUM);
	GetPowerMag();
}

