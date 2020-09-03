
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
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(2000)
#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 10000 )
char *buff = "hello world";
unsigned int data;
int a=100;

static void vSenderTask(void *pvParameters);
static void vSenderTask2(void *pvParameters);
static void vReceiverTask(void *pvParameters);

static void vSenderptrTask(void *pvParameters);
static void vReceiverptrTask(void *pvParameters);

static xQueueHandle queue;
static xQueueHandle ptrqueue;

static void vSenderptrTask(void *pvParameters)
{
  char *_data = "nguyen huynh tai";
  portBASE_TYPE _status;
  my_printf(USART2," Task send ptr to queue start \n\r");
  while (1)
  { 
  _status = xQueueSendToBack(ptrqueue,&_data,0);
    if(_status == pdPASS)
    {
      my_printf(USART2," Send ptr to queue success\n\r");
    }
        //taskYIELD();
  }
}

static void vReceiverptrTask(void *pvParameters)
{
  char *_data;
  portBASE_TYPE _status;
  my_printf(USART2,"Task receiver from queue start \n\r");
  while (1)
  {
    _status = xQueueReceive(ptrqueue,&_data,100);
    if (_status == pdPASS)
    {
      my_printf(USART2," receiver ptr from queue success \n\r");
      my_printf(USART2," data ptr : %s \n\r",_data);
    }
    //taskYIELD();
  }
  

}


static void vSenderTask(void *pvParameters)
{
    int _data = (int)pvParameters;
    portBASE_TYPE _status;
    while (1)
    {
        _status = xQueueSendToBack(queue,&_data,100);
        if(_status != pdPASS)
        {
          my_printf(USART2,"Sender: cannot send to queue \n\r");
        }        
        //taskYIELD();
    }
    
}
static void vSenderTask2(void *pvParameters)
{
    int _data = (int)pvParameters;
    portBASE_TYPE _status;
    while (1)
    {
        _status = xQueueSendToBack(queue,&_data, 1000);
        if(_status != pdPASS)
        {
          my_printf(USART2,"Sender: cannot send to queue \n\r");
        }
    
        //taskYIELD();
    }
    
}
static void vReceiverTask(void *pvParameters)
{
    int _data;
//    portTickType tick_wait;
    portBASE_TYPE _status;
//    tick_wait = 100/portTICK_RATE_MS;
    _data = uxQueueMessagesWaiting(queue);

    my_printf(USART2,"number %d data in queue \n\r",_data);
    
    while (1)
    {    
    _status = xQueueReceive(queue,&_data,0);  
      if(_status == pdPASS)
      {
        my_printf(USART2,"Receiver : %d \n\r",_data);
      }
      else if (_status == errQUEUE_EMPTY )
      {
        my_printf(USART2,"Receiver: Buffer empty \n\r");
      }
                                                                                                                        
      taskYIELD();
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
    TimerHandle_t xAutoReloadTimer,xOneShotTimer;
    BaseType_t xTimer1Started, xTimer2Started;
    xAutoReloadTimer = xTimerCreate("Auto Reload",mainAUTO_RELOAD_TIMER_PERIOD,pdTRUE,0,vTimerSoftware);
    xOneShotTimer = xTimerCreate("One Shot ",mainONE_SHOT_TIMER_PERIOD,pdFALSE,0,vTimerOneShot);
    if((xAutoReloadTimer != NULL) && (xOneShotTimer != NULL))
    {
      xTimer1Started = xTimerStart(xAutoReloadTimer,0);
      xTimer2Started = xTimerStart(xOneShotTimer,0);
    }

//    xTaskCreate(vSenderTask,"Sender1",2048,(void *)100,2,NULL);
//    xTaskCreate(vSenderTask2,"Sender2",2048,(void *)200,2,NULL);
//    xTaskCreate(vReceiverTask,"Receiver",2048,NULL,1,NULL);  

    xTaskCreate(vSenderptrTask,"Senderptr",1024,NULL,2,NULL);
    xTaskCreate(vReceiverptrTask,"Receiverptr",1024,NULL,3,NULL);
    data = xPortGetFreeHeapSize();
    vTaskStartScheduler();
    return 0;
}
