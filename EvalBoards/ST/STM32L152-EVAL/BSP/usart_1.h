#include "stm32l1xx.h"



//user function
void usart1_delay(uint32_t);
void usart1_sendbyte(unsigned char * byte, unsigned char len);

//config function
void usart1_rcc_config(void);
void usart1_gpio_config(void);
void usart1_usart_config(void);
void usart1_init(void);
void usart1_sendstring(unsigned char *p);

void usart3_init(void);
void usart3_sendbyte(unsigned char dat);
void usart3_sendstring(unsigned char *p);

void NVIC_Config(void);







