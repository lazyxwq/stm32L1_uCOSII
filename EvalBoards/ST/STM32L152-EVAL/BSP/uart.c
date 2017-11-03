



/**************************************************************
*函 数 名:  LCD_USARTConfiguration
*功能说明:   LCD 串口初始化
*形    参:   u32 bound  波特率
*返 回 值:   无*
*****************************************************************/
void LCD_USARTConfiguration(u32 bound){

  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);  //GPIOB
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

  /* 配置PB10为 USART1Tx  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* 配置PB11为 USART1 Rx  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  USART_Cmd(USART3, ENABLE);

  //Usart1 NVIC ????

}



/**************************************************************
* 函数名         :void USART3_IRQHandler
*描述            :lcd串口数据终端接收
* 入口函数       : 无
* 出口函数       : 无
* 返回           : 无,
*****************************************************************/
u8 RX;
void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //
  {
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
   RX =USART_ReceiveData(USART3);//(USART1->DR);	//?áè??óê?μ?μ?êy?Y

  }
}

/***************************************************************
* 函数名         : void NVIC_Configuration(void)
*描述            : 中断配置函数
* 入口函数       :
返回            ：无
* 返回           : 无
*****************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* NVIC Configuration */
  NVIC_SetVectorTable(~0x0001FFFF & (uint32_t)__section_begin(".intvec"),
                      0x0001FFFF & (uint32_t)__section_begin(".intvec"));


  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}



