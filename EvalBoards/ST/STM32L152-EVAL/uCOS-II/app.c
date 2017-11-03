/*
*********************************************************************************************************


功能说明：使用了UCOSII操作系统，一共创建了三个任务
任务1,LED闪烁周期1000ms
任务2，串口发送固定数据周期2000ms
任务3，spi发送固定数据0x0A,周期2000ms
编写人：张家越
最后修改时间：2016-09-11
联系电话：13277996975


修改说明：修改了无法进入系统中断的现象
增加功能：1.增加了串口3的驱动，2000ms周期输出一条固定的数据
          2.串口3收到什么数据，再打印输出什么数据
					
出现原因：该工程启动文件未使用ST官方的启动文件，而是使用的UCOSII自己编写的启动文件
          在该启动文件中，对中断映射的函数做了重新定义，与ST官方的不同，所以按照ST
					官方的中断函数名，定义中断函数会出现无法进入中断的现象！
					但是UCOSII官方提供了中断的入口函数，在bsp_int.c的420行，所有中断都会进入改函数，
					然后在switch语句中判断对应的中断
					
修改时间：2016-09-14

*********************/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32L
*                                              on the
*
*                                          STM32L152-EVAL
*                                         Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*		          DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
// PB13 14 15  PD7

#include  <includes.h>

#include "usart_1.h"

#define USE_PRINTF

#ifdef USE_PRINTF
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  ch: the char to be send.
  * @param  *f:
  * @retval the char that send out.
  */
#if 0
int fputc(int ch, FILE *f) 
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  }
  USART_SendData(USART1, (uint8_t) ch);

  return ch;
}
#endif

int fputc(int ch,FILE *f)
{
  USART1->SR;  //USART_GetFlagStatus(USART1, USART_FLAG_TC) 解决第一个字符发送失败的问题
  //一个一个发送字符
  USART_SendData(USART1, (unsigned char) ch);
  //等待发送完成
  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);

  return(ch);
}
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  *f
  * @retval the char that received.
  */
int fgetc(FILE *f) 
{
  int ch;
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
  {
  }
  ch = USART_ReceiveData(USART1);

  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  }
  USART_SendData(USART1, (uint8_t) ch);
  return ch;
}
#else
#define printf  
#define scanf
#endif

#define SD_SPI                           SPI2
#define SD_SPI_CLK                       RCC_APB1Periph_SPI2
#define SD_SPI_SCK_PIN                   GPIO_Pin_13                 /* PB.13 */
#define SD_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOB */
#define SD_SPI_SCK_GPIO_CLK              RCC_AHBPeriph_GPIOB
#define SD_SPI_SCK_SOURCE                GPIO_PinSource13
#define SD_SPI_SCK_AF                    GPIO_AF_SPI2
#define SD_SPI_MISO_PIN                  GPIO_Pin_14                 /* PB.14 */
#define SD_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MISO_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define SD_SPI_MISO_SOURCE               GPIO_PinSource14
#define SD_SPI_MISO_AF                   GPIO_AF_SPI2
#define SD_SPI_MOSI_PIN                  GPIO_Pin_15                 /* PB.15 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MOSI_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define SD_SPI_MOSI_SOURCE               GPIO_PinSource15
#define SD_SPI_MOSI_AF                   GPIO_AF_SPI2
#define SD_CS_PIN                        GPIO_Pin_12                  /* PB.12 */
#define SD_CS_GPIO_PORT                  GPIOB                       /* GPIOD */
#define SD_CS_GPIO_CLK                   RCC_AHBPeriph_GPIOD
#define SD_DETECT_PIN                    GPIO_Pin_6                  /* PE.06 */
#define SD_DETECT_EXTI_LINE              EXTI_Line6
#define SD_DETECT_EXTI_PIN_SOURCE        EXTI_PinSource6

#define SD_DETECT_GPIO_PORT              GPIOE                       /* GPIOE */
#define SD_DETECT_GPIO_CLK               RCC_AHBPeriph_GPIOE
#define SD_DETECT_EXTI_PORT_SOURCE       EXTI_PortSourceGPIOE
#define SD_DETECT_EXTI_IRQn              EXTI9_5_IRQn 


#define SD_DUMMY_BYTE   0xFF


#define SD_CS_LOW()     GPIO_ResetBits(SD_CS_GPIO_PORT, SD_CS_PIN)
/** 
  * @brief  Deselect SD Card: ChipSelect pin high   
  */ 
#define SD_CS_HIGH()    GPIO_SetBits(SD_CS_GPIO_PORT, SD_CS_PIN)

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS ------------------ */
static  OS_STK          AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_STK          AppTaskUsartStk[APP_CFG_TASK_USART_STK_SIZE];

static  OS_STK          AppTaskSpiStk[APP_CFG_TASK_SPI_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppEventCreate            (void);
static  void  AppTaskCreate             (void);


static  void  AppTaskStart              (void        *p_arg);
static void SpiTask (void *p_arg);
static void UsartTask (void *p_arg);


void SPI_IO_Init(void);
uint8_t SD_WriteByte(uint8_t Data);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{

#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif

		NVIC_Config();
		BSP_Init(); 

		usart1_sendstring("USART1 IS OK!\r\n");
		usart3_sendstring("USART3 IS OK!\r\n");
	
	
	
    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"          */

    OSTaskCreateExt((void (*)(void *)) AppTaskStart,           /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&AppTaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	OSTaskCreateExt((void (*)(void *)) UsartTask,           /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskUsartStk[APP_CFG_TASK_USART_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_USART_PRIO,
                    (INT16U          ) APP_CFG_TASK_USART_PRIO,
                    (OS_STK         *)&AppTaskUsartStk[0],
                    (INT32U          ) APP_CFG_TASK_USART_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	OSTaskCreateExt((void (*)(void *)) SpiTask,           /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskSpiStk[APP_CFG_TASK_SPI_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_SPI_PRIO,
                    (INT16U          ) APP_CFG_TASK_SPI_PRIO,
                    (OS_STK         *)&AppTaskSpiStk[0],
                    (INT32U          ) APP_CFG_TASK_SPI_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    return (1);
		
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static void UsartTask (void *p_arg)
{

	(void)p_arg;
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 2000);  
                printf("hello world\n");
		usart1_sendstring("usart1 task ok! \r\n");
		usart3_sendstring("usart3 task ok! \r\n");
	}
}


static void SpiTask (void *p_arg)
{

	(void)p_arg;
	SPI_IO_Init();
	
	while(1)
	{
		SD_CS_LOW();
		SD_WriteByte(0xAA);
		SD_CS_HIGH();
		OSTimeDlyHMSM(0, 0, 0, 2000);  
		usart1_sendstring("SPI task ok! \r\n");
		
	}
}


static  void  AppTaskStart (void *p_arg)
{

    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;


   (void)p_arg;
		
                                                   /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */

	if(cpu_clk_freq ==  16000000)
	{
		usart1_sendstring("system clock is 16MHz! \r\n");
	}
	else
	{
		usart1_sendstring("ERROR,system clock is not 16MHz! \r\n");
	}
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OS_TICKS_PER_SEC;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize memory managment module                   */
    Math_Init();                                                /* Initialize mathematical module                       */


#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif

    APP_TRACE_DBG(("\n\n\r"));
    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppEventCreate();                                          /* Create Application Events                            */

    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                           /* Create application tasks                             */

    while (DEF_TRUE) 
	{      
							/* Task body, always written as an infinite loop.       */
        BSP_LED_On(1);                                      /* Toggle LEDs                                          */
        OSTimeDlyHMSM(0, 0, 0, 500);                            /* Delay for 100 milliseconds                 */      
        BSP_LED_Off(1);                                      /* Toggle LEDs                                          */
        OSTimeDlyHMSM(0, 0, 0, 500);                            /* Delay for 100 milliseconds   */
		
		
    }
}

/*
*********************************************************************************************************
*                                      AppTaskEventCreate()
*
* Description : Create the application Events
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TasStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppEventCreate (void)
{
}


/*
*********************************************************************************************************
*                                      AppTaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TasStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
  
}


void SPI_IO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  /*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO 
       and SD_SPI_SCK_GPIO Periph clock enable */
  RCC_AHBPeriphClockCmd(SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
                        SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

  /*!< SD_SPI Periph clock enable */
  RCC_APB1PeriphClockCmd(SD_SPI_CLK, ENABLE); 

  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

  /* Connect PXx to SD_SPI_SCK */
  GPIO_PinAFConfig(SD_SPI_SCK_GPIO_PORT, SD_SPI_SCK_SOURCE, SD_SPI_SCK_AF);

  /* Connect PXx to SD_SPI_MISO */
  GPIO_PinAFConfig(SD_SPI_MISO_GPIO_PORT, SD_SPI_MISO_SOURCE, SD_SPI_MISO_AF); 

  /* Connect PXx to SD_SPI_MOSI */
  GPIO_PinAFConfig(SD_SPI_MOSI_GPIO_PORT, SD_SPI_MOSI_SOURCE, SD_SPI_MOSI_AF);  
  
  /*!< SD_SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SD_SPI, &SPI_InitStructure);
  
  SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
}


uint8_t SD_ReadByte(void)
{
  uint8_t Data = 0;
  
  /*!< Wait until the transmit buffer is empty */
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
  }
  /*!< Send the byte */
  SPI_I2S_SendData(SD_SPI, SD_DUMMY_BYTE);

  /*!< Wait until a data is received */
  while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  /*!< Get the received data */
  Data = (uint8_t)SPI_I2S_ReceiveData(SD_SPI);

  /*!< Return the shifted data */
  return Data;
}


uint8_t SD_WriteByte(uint8_t Data)
{
  /*!< Wait until the transmit buffer is empty */
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
  }
  
  /*!< Send the byte */
  SPI_I2S_SendData(SD_SPI, Data);
  
  /*!< Wait to receive a byte*/
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  
  /*!< Return the byte read from the SPI bus */ 
  return (uint8_t)SPI_I2S_ReceiveData(SD_SPI);
}

