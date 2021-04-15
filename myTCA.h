#ifndef MYTCA_H
#define MYTCA_H

#define TCA_CTRLA (*((volatile unsigned char *)0xa00))
#define TCA_CTRLB (*((volatile unsigned char *)0xa01))
#define TCA_CTRLC (*((volatile unsigned char *)0xa02))
#define TCA_CTRLD (*((volatile unsigned char *)0xa03))
#define TCA_PERL (*((volatile uint16_t *)0xa26))
#define TCA_PERH (*((volatile unsigned char *)0xa27))
#define TCA_EVCTRL (*((volatile unsigned char *)0xa09))
#define TCA_INTCTRL (*((volatile unsigned char *)0xa0a))
#define TCA_INTFLAGS (*((volatile unsigned char *)0xa0b))

#define TCA_EVCTRL (*((volatile unsigned char *)0xa09))


void TCA_init()
{
  TCA_INTCTRL = B00000001;     // interrupt enable
  TCA_CTRLB = B00000000;       // normal mode
  TCA_PERL = 664;              // top vale 664 (2kHz frequency)
  TCA_CTRLA = B00000011;       // TCA enable, clock div2
}

#endif
