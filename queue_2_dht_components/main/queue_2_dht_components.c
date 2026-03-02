#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "dht.h"
#include "driver/gpio.h"

#define led GPIO_NUM_2
#define DHTPIN GPIO_NUM_4
#define DHTTYPE DHT_TYPE_DHT11
#define TEMP_LIMIT 27
static const char*TAG="DHT";

typedef struct {
    float temp;
    float humi;
    int valid;
} SensorData;

QueueHandle_t myQueue;
void dht_task(void*pvParameters){

    SensorData data;
    while (1)
    {
        esp_err_t ret = dht_read_float_data(DHT_TYPE_DHT11,DHTPIN,&data.humi,&data.temp);
        if(ret==ESP_OK){
            data.valid=1;
            ESP_LOGI(TAG,"ESP OK\n");
        }
        else {
            data.valid=0;
            ESP_LOGI(TAG,"ESP FAILED\n");

        }
        xQueueSend(myQueue,&data,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(2000));
        /* code */
    }
    
}
void remote_task(void*pvParameters){
    int fail_count=0;
    SensorData received;
    while (1)
    {
        if(xQueueReceive(myQueue,&received,portMAX_DELAY)==pdPASS){
            if(received.valid){
                fail_count=0;
                ESP_LOGI(TAG,"TEMP: %.1f C | HUMI: %.1f %%",received.temp,received.humi);
                if(received.temp>TEMP_LIMIT){
                    gpio_set_level(led,1);
                    vTaskDelay(pdMS_TO_TICKS(1000));

                }
                else {
                    gpio_set_level(led,0);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
            else {
                fail_count++;
                if(fail_count>3){
                    ESP_LOGI(TAG,"DHT FAIL 3 TIMES");
                    fail_count=0;
                }
            }
        }
        /* code */
    }
    

}
void app_main(void){
    gpio_reset_pin(led);
    gpio_set_direction(led,GPIO_MODE_OUTPUT);
    myQueue=xQueueCreate(5,sizeof(SensorData));
    xTaskCreate(dht_task,"DHT SENSOR",4096,NULL,6,NULL);
    xTaskCreate(remote_task,"REMOTE",4096,NULL,6,NULL);

}