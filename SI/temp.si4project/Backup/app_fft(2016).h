#ifndef _APP_FFT_H_
#define _APP_FFT_H_

#define NPT   (1024)  //采样点数
#define PI2   (6.28318530717959)
#define PI    (3.141592653589795)
#define Fs    ((float)256)   //采样频率

void InitBufInArray(void);
void GetPowerMag(void);
void FFT_test(void);


#endif

