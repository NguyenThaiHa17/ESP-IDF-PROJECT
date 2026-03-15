#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "h04.h"
#include "system_event.h"

#define ECHO_GPIO GPIO_NUM_18
#define TRIG_GPIO GPIO_NUM_5

extern QueueHandle_t myqueue;

void h04_init(){
    gpio_config_t trig_io={
    .pin_bit_mask=(1ULL << TRIG_GPIO),
    .mode=GPIO_MODE_OUTPUT,
    .pull_down_en=0,
    .pull_up_en=0,
    .intr_type=GPIO_INTR_DISABLE
    };
    gpio_config(&trig_io);

    gpio_config_t echo_io={
        .pin_bit_mask=(1ULL << ECHO_GPIO),
        .mode=GPIO_MODE_INPUT,
        .pull_down_en=0,
        .pull_up_en=0,
        .intr_type=GPIO_INTR_DISABLE
    };
    gpio_config(&echo_io);

    
}
static float distance_mesurance(){
    gpio_set_level(TRIG_GPIO,0);
    esp_rom_delay_us(2);
    gpio_set_level(TRIG_GPIO,1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_GPIO,0);
    int64_t  start;
    int64_t end;
    while (gpio_get_level(ECHO_GPIO)==0);
    start=esp_timer_get_time();
    while (gpio_get_level(ECHO_GPIO)==1);
    end=esp_timer_get_time();
    
    float duration=end-start;
    float distance= duration * 0.034/2;
    return distance;
    
}
void h04_task(void*pvParameters){
  system_event_t event;
    h04_init();
    while (1)
    {
        float distance=distance_mesurance();
        event.type=EVENT_DISTANCE;
        event.distance_cm=distance;
        xQueueSend(myqueue,&event,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
       
    }
    
}