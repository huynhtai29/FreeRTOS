
#include "stdio.h"
#include "stdarg.h"
#include <stdlib.h>
#include "string.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "my_printf.h"
#include "timers.h"
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(500)
#define mainAUTO_RELOAD_TIMER_CHANGE pdMS_TO_TICKS(4000)
#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 10000 )
char *buff = "hello world";
int a=100;

static xQueueHandle queue;
static xQueueHandle ptrqueue;
TimerHandle_t xAutoReloadTimer,xOneShotTimer;
BaseType_t xTimer1Started, xTimer2Started;
static void vTimerID(TimerHandle_t xTimer)
{
    TickType_t _ticknow;
    uint32_t _TimerID;
    _TimerID = (uint32_t) pvTimerGetTimerID(xTimer);
    _TimerID++;
    vTimerSetTimerID(xTimer,(void *)_TimerID);
    _ticknow = xTaskGetTickCount();
    if (xTimer == xAutoReloadTimer)
    {
      GPIO_ToggleBits(GPIOD,GPIO_Pin_12);
      my_printf(USART2,"AutoReload ID : %d Tick : %d\n\r",_TimerID,_ticknow);
      if(_TimerID == 5)
      {
        //xTimerStop(xTimer,0);
        //xTimerChangePeriod(xTimer,mainAUTO_RELOAD_TIMER_CHANGE,0);
      }
    }
    else
    {
      GPIO_ToggleBits(GPIOD,GPIO_Pin_13);
      my_printf(USART2,"OneShot Timer ID : %d Tick : %d\n\r",_TimerID,_ticknow);
    }
    
    
  
}
static void vTimerSoftware(TimerHandle_t xTimer)
{
  GPIO_ToggleBits(GPIOD,GPIO_Pin_12);
}
static void vTimerOneShot(TimerHandle_t xTimer)
{
  GPIO_SetBits(GPIOD,GPIO_Pin_13);
}
int main(void)
{
    int data = pvPortMalloc(sizeof(int));  
    my_printf_config();
    queue = xQueueCreate(5,sizeof(int));
    if(queue == NULL)
    {
      return 1;
    }
    ptrqueue = xQueueCreate(3,sizeof(char *));
    if(ptrqueue == NULL)
    {
      return 1;
    }
    
    xAutoReloadTimer = xTimerCreate("Auto Reload",mainAUTO_RELOAD_TIMER_PERIOD,pdTRUE,0,vTimerID);
    xOneShotTimer = xTimerCreate("One Shot ",mainONE_SHOT_TIMER_PERIOD,pdFALSE,0,vTimerID);
    if((xAutoReloadTimer != NULL) && (xOneShotTimer != NULL))
    {
      xTimer1Started = xTimerStart(xAutoReloadTimer,0);
      xTimer2Started = xTimerStart(xOneShotTimer,0);
    }
    data = xPortGetFreeHeapSize();
    vTaskStartScheduler();
    return 0;
}

void EXTI0_IRQHandler(void)
{
      if(EXTI_GetITStatus(EXTI_Line0) != RESET){
        GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
        // Clear the interrupt flag
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
void USART2_IRQHandler(void) 
{

  if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!=RESET)
  {
      char tmp = USART_ReceiveData(USART2); 
      if(tmp == '1')
      {
        GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
      }
      USART_SendData(USART2, tmp);
  }
}