#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define led GPIO_NUM_2
#define button GPIO_NUM_5

void app_main(void)
{
 gpio_config_t io_conf={};
 io_conf.pin_bit_mask=1ULL<<led;
 io_conf.pull_up_en=0;
 io_conf.pull_down_en=0;
 io_conf.mode=GPIO_MODE_OUTPUT;
 io_conf.intr_type=GPIO_INTR_DISABLE;
 gpio_config(&io_conf);
    io_conf.pin_bit_mask=1ULL<<button;
    io_conf.mode=GPIO_MODE_INPUT;
    io_conf.intr_type=GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    int ledState=0;
    while (1)
    {
              if (gpio_get_level(button) == 0) {   // nhấn = LOW
            vTaskDelay(pdMS_TO_TICKS(20));

        if(gpio_get_level(button)==0){
            ledState=!ledState;

            gpio_set_level(led,ledState);
            printf("Da Nhan NUT: %d",ledState);
        
        while(gpio_get_level(button) == 0) {
                    vTaskDelay(10 / portTICK_PERIOD_MS);

        }
    }
}
    vTaskDelay(pdMS_TO_TICKS(10));
}
}
