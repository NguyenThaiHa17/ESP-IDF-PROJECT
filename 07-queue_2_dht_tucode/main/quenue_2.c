#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "dht.h"


#define DHT_PIN GPIO_NUM_4
#define DHT_TYPE DHT_TYPE_DHT11



typedef struct {
    float temperature;
    float humidity;
} SensorData;
QueueHandle_t myQueue;
void dht_task(void*pvParameters){
    SensorData data;
    while (1)
    {
        if(dht_read_float_data(DHT_TYPE,DHT_PIN,&data.humidity,&data.temperature)==ESP_OK){
            printf("DHT OK\n");
        
        if(xQueueSend(myQueue,&data,pdMS_TO_TICKS(100))!=pdPASS){
            printf("queue full\n");

        }
    }
        else {
            printf("dht failed\n");
        }
vTaskDelay(pdMS_TO_TICKS(2000));
        }
        /* code */
    }
   

void received_task(void*pvParameters){
    SensorData received;
    while (1)
    {
        if(xQueueReceive(myQueue,&received,pdMS_TO_TICKS(3000))==pdPASS)
        {
            printf("TEMP:%.1lf  HUMI:%.1lf\n",received.temperature,received.humidity);
        }
        else {
            printf("timeout\n");
        }
        /* code */
    }
}
void app_main(void){
    myQueue=xQueueCreate(5,sizeof(SensorData));
    xTaskCreate(dht_task,"DHT",4096,NULL,5,NULL);
    xTaskCreate(received_task,"Received",4096,NULL,5,NULL);
}
