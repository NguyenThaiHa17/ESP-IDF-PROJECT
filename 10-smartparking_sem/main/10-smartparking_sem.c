#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include  "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#define TRIG_PIN GPIO_NUM_5
#define ECHO_PIN GPIO_NUM_18

QueueHandle_t myqueue;
SemaphoreHandle_t mysemphr;
void hc04_init (){
    gpio_config_t trig_conf={
        .pin_bit_mask=(1ULL << TRIG_PIN),
        .mode=GPIO_MODE_OUTPUT,
        .pull_down_en=0,
        .pull_up_en=0,
        .intr_type=GPIO_INTR_DISABLE
    };
 gpio_config(&trig_conf);
gpio_config_t echo_conf={
     .pin_bit_mask=(1ULL << ECHO_PIN),
        .mode=GPIO_MODE_INPUT,
        .pull_down_en=0,
        .pull_up_en=0,
        .intr_type=GPIO_INTR_DISABLE
};
gpio_config(&echo_conf);
}
 static float distance_measure(){
    gpio_set_level(TRIG_PIN,0);
    esp_rom_delay_us(2);
    gpio_set_level(TRIG_PIN,1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_PIN,0);
    int64_t  start;
    int64_t end;
    while (gpio_get_level(ECHO_PIN)==0);
    start=esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN)==1);
    end=esp_timer_get_time();
    
    float duration=end-start;
    float distance= duration * 0.034/2;
    return distance;
 
    
}
void send_task(void *pvParameters){
    float distance;
    while (1)
    {
        distance=distance_measure();
        printf("Sensor : %.2f\n",distance);
        xQueueSend(myqueue,&distance,0);
        xSemaphoreGive(mysemphr);
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* code */
    }
    
}
void received_task(void *pvParameters){
    float data;
    while (1)
    {
        if(xSemaphoreTake(mysemphr,portMAX_DELAY)){
            xQueueReceive(myqueue,&data,portMAX_DELAY);
            if(data<20){
                printf("CO XE\n");

            }
            else if (data > 20 ) {
                printf("KHONG CO XE\n");
            }
        }
        /* code */
    }
    
}
void app_main(void)
{
    hc04_init();
    myqueue=xQueueCreate(10,sizeof(float));
    mysemphr=xSemaphoreCreateBinary();
    xTaskCreate(send_task,"SEND",4096,NULL,5,NULL);
    xTaskCreate(received_task,"RECEIVED",4096,NULL,4,NULL);
}
