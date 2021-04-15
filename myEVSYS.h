#ifndef MYEVSYS_H
#define MYEVSYS_H

#define EVSYS_CH0 (*((volatile unsigned char *)0x190))
#define EVSYS_ADC (*((volatile unsigned char *)0x1a8))

void EVSYS_init()
{
  EVSYS_CH0 = B10000000;  // TCA event generator로 사용
  EVSYS_ADC = B00000001;  // ADC를 TCA event user로 등록
}


#endif
