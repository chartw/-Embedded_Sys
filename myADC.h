#ifndef MYADC_H
#define MYADC_H

//ADC register
#define ADC_CTRLA (*((volatile unsigned char *)0x600))
#define ADC_CTRLC (*((volatile unsigned char *)0x602))
#define ADC_MUXPOS (*((volatile unsigned char *)0x606))
#define ADC_COMMAND (*((volatile unsigned char *)0x608))
#define ADC_INTCTRL (*((volatile unsigned char *)0x60a))
#define ADC_INTFLAGS (*((volatile unsigned char *)0x60b))
#define ADC_RESL (*((volatile unsigned char *)0x610))
#define ADC_EVCTRL (*((volatile unsigned char *)0x609))

uint8_t ADC_init_light()    //초기화시, 최초 조도 기준값 측정
{
  ADC_CTRLC = B00010010;    // Vref=VDD, 8 prescale
  ADC_CTRLA = B00000101;    // 8bits resolution, ADC Enable
  ADC_INTCTRL = B00000000;
  ADC_MUXPOS = B00000101;   // use pd5 pin for adc(photo diode)

  uint8_t l;
  uint8_t prevl = 0;
  for (int i = 0; i < 50; i++)
  {
    ADC_COMMAND = B00000001;
    while (!(ADC_INTFLAGS & B00000001)); //if result is empty, wait
    ADC_INTFLAGS = B00000001;
    l = ADC_RESL;
    prevl = ((1 - 0.7) * l) + (0.7 * prevl);
  }
  return prevl;
}

void ADC_init()   // 조도값 측정후, event input start
{
  ADC_EVCTRL = B00000001;   // event input start
  ADC_MUXPOS = B00000110;   //use pd6 pin for adc(mic)
}

uint8_t readS()   // 마이크 센서값 conversion
{
  ADC_MUXPOS = B00000110;   //use pd6 pin for adc(photo diode)
  while (!(ADC_INTFLAGS & B00000001)); //if result is empty, wait
  ADC_INTFLAGS = B00000001;
  return ADC_RESL;
}

uint8_t readL()   // 포토 다이오드 값 convsersion
{
  ADC_MUXPOS = B00000101;   //use pd5 pin for adc
  while (!(ADC_INTFLAGS & B00000001)); //if result is empty, wait
  ADC_INTFLAGS = B00000001;
  return ADC_RESL;
}

#endif
