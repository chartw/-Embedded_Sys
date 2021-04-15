#include "myUART.h"
#include "myADC.h"
#include "myTCB.h"
#include "myTCA.h"
#include "myFFT.h"
#include "myEVSYS.h"
#include "myPORT.h"

#include <avr/eeprom.h>

// 전역 변수 선언
#define samples 256     // 샘플링 개수
#define samplef 2000    // 샘플링 주파수
double sampler = ((double)samplef / (double)samples); //샘플링된 데이터 주파수 간격
double sReal[samples];    //샘플링 데이터 실수부
double sImag[samples];    //샘플링 데이터 허수부

volatile uint8_t luxdata[2] = {0};    //조도 데이터
volatile uint8_t lightdata[3] = {0};  //LED 밝기 데이터

uint8_t initL = 0;            //최초 조도값 측정변수
volatile uint8_t lightL = 0;  //LED 밝기 레벨

uint8_t light;                //포토 다이오드 센서 측정값
volatile uint8_t prevlight;   //지수 이동 평균 계산을 위한 이전 평균값
double k = 0.7;               //지수 이동 평균 계수

uint8_t currentF;       //Fflag의 현재값
uint8_t lastF = 0;      //Fflag의 이전값
int Lflag = 0;          //LED가 켜져있는지 꺼져있는지 확인하는 flag

// 함수 프로토 타입 선언
void PrintVector(double *vData, uint16_t bufferSize);     //벡터 출력 함수
void eeprom_init();   // EEPROM 초기 read,write


//ISR
ISR(TCA0_OVF_vect)    //TCA overflow interrput(250us 주기)
{
  //  PORTA_OUTTGL = B10000000; //PORTA pin7 led 토글
  TCA_INTFLAGS = B00000001;
}

ISR(PORTE_PORT_vect)  //PORTE interrupt(버튼 입력 인터럽트)
{
  if ((PORTE_INTFLAGS & B00000001) == 1)  //UP 버튼 입력시 lightL 증가
  {
    if (lightL < 3) lightL++;
    PORTE_INTFLAGS |= B00000001;
  }
  else if (((PORTE_INTFLAGS & B00000010) >> 1) == 1)  //Down 버튼 입력시 lightL 감소
  {
    if (lightL != 0) lightL--;
    PORTE_INTFLAGS |= B00000010;
  }

  if (prevlight < luxdata[0])       //현재 조도값에 따라 전역변수와, EEPROM에 LED밝기 레벨 저장
  {
    lightdata[0] = lightL;
    eeprom_write_byte(5, lightL);
  }
  else if (prevlight < luxdata[1])
  {
    lightdata[1] = lightL;
    eeprom_write_byte(6, lightL);
  }
  else
  {
    lightdata[2] = lightL;
    eeprom_write_byte(7, lightL);
  }
}



int main(void)
{
  //setup
  EVSYS_init();
  TCA_init();
  TCB0_init();
  UART_init();
  PORT_init();
  initL = ADC_init_light();
  ADC_init();
  sei();

  eeprom_init();

  //loop
  while (1)
  {
    int Vflag = 0;    //일정 볼륨 이상 입력되었는지 확인 flag
    int Fflag = 0;    //특정 주파수가 입력되었는지 확인 flag
    double Mf = 0;    //Major frequency
    sReal[0] = readS();                       //채널 변경을 위해 1회 컨버젼
    for (uint16_t i = 0; i < samples; i++)    //소리 시간축 데이터 샘플링 시작
    {
      sReal[i] = readS();                     //마이크 데이터 컨버젼
      if (sReal[i] > 20) Vflag = 1;           //노이즈로 인한 오차 제거
      sImag[i] = 0;
    }

    myFFT(sReal, sImag, samples);     //fft 계산
    Magnitude(sReal, sImag, samples); //복소수 magnitude 계산
    uint8_t p[5];                     //peak 5개가 저장될 변수
    peakes(sReal, samples, p, 5);       //peak 5개 판별

    light = readL();          //채널 변경을 위해 1회 컨버젼
    light = readL();          //포토 다이오드 데이터 컨버젼
    prevlight = ((1 - k) * light) + (k * prevlight);  //지수 이동 평균 계산

    if (Vflag == 1)   //노이즈 제거
    {
      Mf = MajorPeak(sReal, samples, samplef, p, 5, 8) * sampler; //Major frequency 계산
      if (Mf == 500) Fflag = 1;               //Major frequency가 500이면, Fflag=1;
    }

    if (Fflag != lastF)     //연속된 입력에 반응하지 않도록 함
    {
      currentF = Fflag;
      if (currentF == 1)    //만약 특정 주파수가 입력 되었다면
      {
        Lflag = !Lflag;           //Lflag 토글
        PORTA_OUTTGL = B10000000; //PORTA pin7 led 토글
      }
    }

    if (Lflag == 1)         //Lflag가 1이면 자동 밝기 조절
    {
      if (prevlight < luxdata[0]) lightL = lightdata[0];
      else if (prevlight < luxdata[1]) lightL = lightdata[1];
      else lightL = lightdata[2];

      if (lightL == 3) pwmset(255);
      else if (lightL == 2) pwmset(120);
      else if (lightL == 1) pwmset(20);
      else if (lightL == 0) pwmset(0);
    }
    else pwmset(0);         //Lflag가 0이면 LED OFF

    lastF = Fflag;
  }
}



void PrintVector(double *vData, uint16_t bufferSize)
{
  for (int i = 0; i < 20; i++) sendstring('\0');
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    uint16_t val_temp = Map(vData[i], 0, 1000, 0, 255);
    sendstring(val_temp);
    sendstring('\0');
  }
}

void eeprom_init()
{
  if (initL >= 20 && initL <= 100)
  {
    luxdata[0] = 10;
    luxdata[1] = initL + 50;
    eeprom_write_byte(1, luxdata[1]);
  }
  else if (initL > 100 && initL <= 200)
  {
    luxdata[0] = 10;
    luxdata[1] = initL + 30;
    eeprom_write_byte(1, luxdata[1]);
  }
  else                                  //최초 조도 측정시 너무 작거나, 너무 클경우 기준값은 EEPROM의 데이터를 이용
  {
    luxdata[0] = 10;
    luxdata[1] = eeprom_read_byte(1);
  }
  lightdata[0] = eeprom_read_byte(5);
  lightdata[1] = eeprom_read_byte(6);
  lightdata[2] = eeprom_read_byte(7);
}
