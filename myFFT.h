#ifndef MYFFT_h
#define MYFFT_h

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define twoPi 6.28318531


void Magnitude(double *vReal, double *vImag, uint16_t samples);                       //복소수 magnitude 계산
void myFFT(double *vReal, double *vImag, uint16_t samples);                           //FFT 계산
uint8_t Exponent(uint16_t value);                                                     //2의 몇승인지 계산
void Swap(double *x, double *y);
void peakes(double *vD, uint16_t samples, uint8_t *p);                                //max peak 5개 판별
uint16_t MajorPeak(double *vD, uint16_t samples, uint16_t samplef, uint8_t *p);       //Major peak 판별
double Map(double x, uint16_t inmin, uint16_t inmax, uint16_t outmin, uint16_t outmax);// 데이터 범위 맵핑 함수


void myFFT(double *vReal, double *vImag, uint16_t samples)    //FFT 계산
{
  uint16_t j = 0;
  for (uint16_t i = 0; i < (samples - 1); i++) {      //Radix-2 연산 순서에 맞게 비트 역전
    if (i < j) {
      Swap(&vReal[i], &vReal[j]);
      Swap(&vImag[i], &vImag[j]);
    }
    uint16_t k = (samples >> 1);
    while (k <= j) {
      j -= k;
      k >>= 1;
    }
    j += k;
  }

  uint8_t power = Exponent(samples);
  double c1 = -1.0;
  double c2 = 0.0;
  uint16_t l2 = 1;
  for (uint16_t l = 0; (l < power); l++)
  {
    uint16_t l1 = l2;
    l2 <<= 1;
    double u1 = 1.0;
    double u2 = 0.0;
    for (j = 0; j < l1; j++)
    {
      for (uint16_t i = j; i < samples; i += l2)
      {
        uint16_t i1 = i + l1;
        double t1 = u1 * vReal[i1] - u2 * vImag[i1];
        double t2 = u1 * vImag[i1] + u2 * vReal[i1];
        vReal[i1] = vReal[i] - t1;
        vImag[i1] = vImag[i] - t2;
        vReal[i] += t1;
        vImag[i] += t2;
      }
      double z = ((u1 * c1) - (u2 * c2));
      u2 = ((u1 * c2) + (u2 * c1));
      u1 = z;
    }
    c2 = sqrt((1.0 - c1) / 2.0);

    c2 = -c2;

    c1 = sqrt((1.0 + c1) / 2.0);
  }
}

uint8_t Exponent(uint16_t value)        //value가 2의 몇승인지 계산
{
  uint8_t result = 0;
  while (value != 1)
  {
    value = value / 2;
    result++;
  }
  return result;
}

void Swap(double *x, double *y)
{
  double temp = *x;
  *x = *y;
  *y = temp;
}


void Magnitude(double *vReal, double *vImag, uint16_t samples)    //복소수의 Magnitude 계산
{
  /* vM is half the size of vReal and vImag */
  for (uint16_t i = 0; i < (samples >> 1); i++)
  {
    vReal[i] = sqrt(pow(vReal[i], 2) + pow(vImag[i], 2));
  }
  for (uint16_t i = (samples >> 1); i < samples; i++)
  {
    vReal[i] = 0;
  }
}

void peakes(double *vD, uint16_t samples, uint8_t *p, uint8_t n)         //최대 peak n개 판별
{
  double Max[n] = {0};
  for (uint16_t i = 0; i < ((samples >> 1)); i++)
  {
    if ((vD[i - 1] < vD[i]) && (vD[i] > vD[i + 1]))       //vD[i]가 peak인 경우
    {
//      for (int j = 0; j < n; j++)
//      {
//        if (vD[i] > Max[j])               //vD[i]가 Max[j]보다 크면
//        {
//          for (int l = n - 1; l > j; l--)   //Max[j]에 vD[i]를 저장하기 위해 이전의 값 right shift
//          {
//            Max[l] = Max[l - 1];
//            p[l] = p[l - 1];
//          }
//          Max[j] = vD[i];                 //Max[j]에 vD[i], p[j]에 i 저장
//          p[j] = i;
//          break;                          //최종적으로 Max[0]부터 Max[n-1]까지 내림차순으로 Max peak 정렬
//        }
//      }
           if (vD[i] > Max[0])
            {
              Max[0]=vD[i];
              p[0]=i;
            }
            else if (vD[i] > Max[1])
            {
              Max[1] = vD[i];
              p[1] = i;
            }
            else if (vD[i] > Max[2])
            {
              Max[2] = vD[i];
              p[2] = i;
            }
            else if (vD[i] > Max[3])
            {
              Max[3] = vD[i];
              p[3] = i;
            }
            else if (vD[i] > Max[4])
            {
              Max[4] = vD[i];
              p[4] = i;
            }
    }
  }
}


uint16_t MajorPeak(double *vD, uint16_t samples, uint16_t samplef, uint8_t *p, uint8_t pn, uint8_t n)   //Major peak 판별
{
  uint16_t sum[pn] = {0};
  uint16_t maxsum = 0;
  uint8_t maxindex = 0;
  for (uint8_t i = 0; i < 5; i++)
  {
    uint16_t pt = p[i] + n;             //peak 인덱스의 전후 n개의 데이터 합산
    for (uint8_t j = 0; j < 2 * n + 1; j++)
    {
      sum[i] = sum[i] + vD[pt];
      if (pt == 0) break;               //만약 인덱스가 0에 도달할경우 합산 종료
      pt--;
    }
    if (sum[i] > maxsum)                //최대값 판별
    {
      maxsum = sum[i];
      maxindex = p[i];
    }
  }
  return maxindex;
}

double Map(double x, uint16_t inmin, uint16_t inmax, uint16_t outmin, uint16_t outmax)    //데이터 범위 맵핑 함수
{
  return (x - inmin) * (outmax - outmin) / (inmax - inmin) + outmin;
}


#endif
