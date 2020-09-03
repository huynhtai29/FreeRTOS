
#include "stdio.h"
#include "stdarg.h"
#include <stdlib.h>
#include "string.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
char *buff = "hello world";
unsigned int data;
int a=100;
void Board_Configuration(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Configure output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /*Uart 2  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // PA2 is connected to high, so use pulldown resistor
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // PA3 is connected to high, so use pulldown resistor
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    // uart
    USART_DeInit(USART2);
    USART_InitStructure.USART_BaudRate = 115200; // Saniye içinde hat üzerinden kaç tane bit gönderilmesi gerektigi
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
    USART_LINBreakDetectLengthConfig(USART2, USART_LINBreakDetectLength_11b);
    USART_LINCmd(USART2, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    //	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_FE, ENABLE);
}

static void TaskA(void *pvParameters)
{  
  int *_data = pvPortMalloc(sizeof(int));
    for(;;) 
    {
      (*_data)++;
      GPIO_SetBits(GPIOD, GPIO_Pin_12);
      vTaskDelay(1000);
      GPIO_ResetBits(GPIOD, GPIO_Pin_12);
      vTaskDelay(1000);
      my_printf(USART2,"%d\n\r",*_data);
    }
}
static void TaskB(void *pvParameters)
{  
    for(;;) 
    {
      GPIO_SetBits(GPIOD, GPIO_Pin_13);
      vTaskDelay(900);
      GPIO_ResetBits(GPIOD, GPIO_Pin_13);
      vTaskDelay(900);
    }
}
static void TaskC (void *pvParameters)
{
  for(;;)
  {
    GPIO_SetBits(GPIOD,GPIO_Pin_14);
    vTaskDelay(800);
    GPIO_ResetBits(GPIOD,GPIO_Pin_14);
    vTaskDelay(800);
//    USART_SendString(USART2,"nguyen huynh tai \n\r");

  }

}
static void TaskD (void *pvParameters)
{
  for (;;)
  { 
    GPIO_SetBits(GPIOD,GPIO_Pin_15);
    vTaskDelay(700);
    GPIO_ResetBits(GPIOD,GPIO_Pin_15);
    vTaskDelay(700);
    //my_printf(USART2,"nguyen huynh tai1 %s %d \n\r",buff,a);
  }
  
}

int main(void)
{
  
  Board_Configuration();
  xTaskCreate(TaskA, (signed char*)"TaskA", 128, NULL, 4, NULL);
  xTaskCreate(TaskB, (signed char*)"TaskB", 128, NULL, 4, NULL);
  xTaskCreate(TaskC, (signed char*)"TaskC", 128, NULL, 4, NULL);
  xTaskCreate(TaskD, (signed char*)"TaskD", 128, NULL, 4, NULL);
  data = xPortGetFreeHeapSize();
  vTaskStartScheduler();
  return 0;
}
void my_printf(USART_TypeDef* USARTx,char *buff, ...)
{
  va_list ap;
  char *p,*sval;
  int ival;
  va_start(ap,buff);

  for(p = buff;*p;p++)
  {
    if(*p != '%')
    {
      USART_SendData(USART2,*p);
      continue;
    }
    switch (*++p)
    {
    case 'd':
        ival = va_arg(ap,int);
        print_int(USART2,ival);
        break;
    case 's':
        sval = va_arg(ap,char *);
        USART_SendString(USART2,sval);
        break;
    default:
      break;
    }
  }
  va_end(ap);
}
void print_int(USART_TypeDef* USARTx,int num)
{
  unsigned long  _val =1;
	int _num = num;
	char i;
	char *buff =&i;
	while(_num !=0)
	{
		_val*=10;
		_num =_num/10;
	}
	_val =_val/10;
	while(_val !=0)
	{
		i = num/_val;
		num = num - i*(_val);
		_val =_val/10;
		i+= 0x30;
		USART_SendData(USART2,i);
	}
}