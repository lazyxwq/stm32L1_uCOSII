#include "stm32l1xx.h"
#include "usart_1.h"



void usart1_init(void)
{
	usart1_rcc_config();
	usart1_gpio_config();
	usart1_usart_config();
	
}


void usart3_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);  //GPIOB
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

  /* ÅäÖÃPB10Îª USART1Tx  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* ÅäÖÃPB11Îª USART1 Rx  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  USART_Cmd(USART3, ENABLE);

}
//gpio config
void usart1_gpio_config(void)
{
	GPIO_InitTypeDef usart1_gpio_initstructure;
	
	//connect PA.9 to usart1's tx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	//connect PA.10 to usart1's rx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* Configure USART Tx as alternate function push-pull */
  usart1_gpio_initstructure.GPIO_Pin = GPIO_Pin_9;
  usart1_gpio_initstructure.GPIO_Mode = GPIO_Mode_AF;
  usart1_gpio_initstructure.GPIO_Speed = GPIO_Speed_40MHz;
  usart1_gpio_initstructure.GPIO_OType = GPIO_OType_PP;
  usart1_gpio_initstructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &usart1_gpio_initstructure);
    
  /* Configure USART Rx as alternate function push-pull */
  usart1_gpio_initstructure.GPIO_Pin =GPIO_Pin_10;
  GPIO_Init(GPIOA,&usart1_gpio_initstructure);
}

//rcc config
void usart1_rcc_config(void)
{
	//enable gpio clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	//enable usart1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

//usart config
void usart1_usart_config(void)
{
	USART_InitTypeDef	usart1_usart_initstructure;
	
	usart1_usart_initstructure.USART_BaudRate = 115200;
  usart1_usart_initstructure.USART_WordLength = USART_WordLength_8b;
  usart1_usart_initstructure.USART_StopBits = USART_StopBits_1;
  usart1_usart_initstructure.USART_Parity = USART_Parity_No;
  usart1_usart_initstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart1_usart_initstructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	//configuration
	USART_Init(USART1, &usart1_usart_initstructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	//enable
	USART_Cmd(USART1, ENABLE);
}

void usart1_delay(uint32_t nTime)
{
  for(; nTime != 0; nTime--);
}

void usart1_sendbyte(unsigned char * byte, unsigned char len)
{
	unsigned char i = 0;
	for(i = 0; i < len; i++)
	{
		USART_SendData(USART1, byte[i]);
		usart1_delay(2000);
	}
}

void usart1_sendstring(unsigned char *p)
{
	while(*p)
	{
		usart1_sendbyte(p,1);
		p++;
	}
}


void usart3_sendbyte(unsigned char dat)
{

	USART_SendData(USART3, dat);
	usart1_delay(2000);
	
}

void usart3_sendstring(unsigned char *p)
{
	while(*p)
	{
		usart3_sendbyte(*p);
		p++;
	}
}

void NVIC_Config(void)
{
//   NVIC_InitTypeDef NVIC_InitStructure;

//   /* Enable the USARTx Interrupt */
//   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//   NVIC_Init(&NVIC_InitStructure);
	
	 NVIC_InitTypeDef NVIC_InitStructure;
  /* NVIC Configuration */
//   NVIC_SetVectorTable(~0x0001FFFF & (uint32_t)__section_begin(".intvec"),
//                       0x0001FFFF & (uint32_t)__section_begin(".intvec"));


  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	 NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}


