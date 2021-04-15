#ifndef MYPORT_H
#define MYPORT_H

#define PORTA_DIR (*((volatile unsigned char *)0x400))
#define PORTA_OUT (*((volatile unsigned char *)0x404))
#define PORTA_OUTTGL (*((volatile unsigned char *)0x407))

#define PORTC_DIR (*((volatile unsigned char *)0x440))

#define PORTE_DIR (*((volatile unsigned char *)0x480))
#define PORTE_OUT (*((volatile unsigned char *)0x484))
#define PORTE_IN (*((volatile unsigned char *)0x488))
#define PORTE_INTFLAGS (*((volatile unsigned char *)0x489))
#define PORTE_PIN0CTRL (*((volatile unsigned char *)0x490))
#define PORTE_PIN1CTRL (*((volatile unsigned char *)0x491))
#define PORTE_PIN2CTRL (*((volatile unsigned char *)0x492))


void PORT_init()
{
   PORTA_DIR = B10000100;       // pin2 : TCB pwm , pin7 : ON/OFF 체크
   PORTA_OUT |= B10000000;      // pin 7 active low
   PORTC_DIR = B00000001;       // USART pin set
   PORTE_PIN0CTRL=B00001011;    // pullup enable, 버튼 interrupt enable (falling edge)
   PORTE_PIN1CTRL=B00001011;
}

#endif
