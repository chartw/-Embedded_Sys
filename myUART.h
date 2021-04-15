#ifndef MYUART_H
#define MYUART_H

//UART register
#define UART_TXDATA (*((volatile unsigned char *)0x822))
#define UART_STATUS (*((volatile unsigned char *)0x824))
#define UART_CTRLB (*((volatile unsigned char *)0x826))
#define UART_CTRLC (*((volatile unsigned char *)0x827))
#define UART_BAUDL (*((volatile unsigned char *)0x828))
#define UART_BAUDH (*((volatile unsigned char *)0x829))


void UART_init();         //initialize UART
void sendChar(char d);    //send character to UART
void sendstring(int val);

void UART_init()
{
  //  UART_BAUDL = 0x2c;
  //  UART_BAUDH = 0x02;

  UART_BAUDL = 0x0b;
  UART_BAUDH = 0x1a;
  UART_CTRLB = B11000000;
  UART_CTRLC = B00000011;
}

void sendChar(char d)
{
  while (!(UART_STATUS & B00100000));
  UART_TXDATA = d;
}

void sendstring(int val)
{
  char buf[10];
  itoa(val, buf, 10);        //integer aval is convert to string

  for (int i = 0; i < 10; i++) //send string
  {
    if (buf[i] == '\0') break;
    sendChar(buf[i]);
  }
  sendChar('\n');
}

#endif
