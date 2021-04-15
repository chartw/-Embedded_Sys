#ifndef MYTCB_H
#define MYTCB_H


#define TCB0_CTRLA (*((volatile unsigned char *)0xa80))
#define TCB0_CTRLB (*((volatile unsigned char *)0xa81))
#define TCB0_EVCTRL (*((volatile unsigned char *)0xa84))
#define TCB0_CNTL (*((volatile int16_t *)0xa8a))
//#define TCB0_CNTL (*((volatile unsigned char *)0xa8a))
//#define TCB0_CNTH (*((volatile unsigned char *)0xa8b))

//#define TCB0_CCMPL (*((volatile uint16_t *)0xa8c))
#define TCB0_CCMPL (*((volatile unsigned char *)0xa8c))
#define TCB0_CCMPH (*((volatile unsigned char *)0xa8d))

#define TCB0_INTCTRL (*((volatile unsigned char *)0xa85))
#define TCB0_INTFLAGS (*((volatile unsigned char *)0xa86))



void TCB0_init()
{
  TCB0_CCMPL = 0xff;        // TOP value
  TCB0_CCMPH = 0xff;        // CMP value
  TCB0_CTRLA = B00000001;   // TCB0 enable
  TCB0_CTRLB = B00010111;   // 8bits PWM mode
}

void pwmset(uint8_t n)      // PWM CMP value set
{
  TCB0_CCMPH = n;
}



#endif
