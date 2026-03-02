#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
QueueHandle_t myQuenue;
void sender_task(void*pvParameters){
    int counter=0;
    while (1)
    {
        xQueueSend(myQuenue,&counter,portMAX_DELAY);
        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* code */
    }
    

}
void receiver_task(void*pvParameters){
    int received;
    while(1){
if(xQueueReceive(myQuenue,&received,portMAX_DELAY)){
    printf("Received: %d\n",received);
}
    
}
}

void app_main(void)
{
    myQuenue =xQueueCreate(5,sizeof(int));
xTaskCreate(sender_task,"Sender",2048,NULL,5,NULL);
xTaskCreate(receiver_task,"Received",2048,NULL,5,NULL);
}