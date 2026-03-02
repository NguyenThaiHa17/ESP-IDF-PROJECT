#include<stdio.h>
#include"freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#define led 2
void blink_led(void*pvParameters){
    gpio_reset_pin(led);
    gpio_set_direction(led,GPIO_MODE_OUTPUT);
    while(1){
        gpio_set_level(led,1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(led,0);
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}
void monitor_heap_task(void*pvParameters){
    while (1)
    {
        printf("Free heap: %lu bytes\n",esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(2000));
        /* code */
    }
    
}
void app_main(void){
    xTaskCreate(monitor_heap_task,"Monitor",2048,NULL,5,NULL);
    xTaskCreate(blink_led,"Blink_led",2048,NULL,5,NULL);
}