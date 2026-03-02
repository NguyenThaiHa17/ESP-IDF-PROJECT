
#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define button 5
#define led 2
void app_main(void)
{
    gpio_reset_pin(led);
    gpio_reset_pin(button);
    gpio_set_direction(led,GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(button,GPIO_PULLUP_ONLY);
    gpio_set_direction(button,GPIO_MODE_INPUT);
    int lastButton_state=1;
    int led_state=0;
    gpio_set_level(led,led_state);
     while(1){
        int current_state=gpio_get_level(button);
        if(lastButton_state==1 && current_state==0){
            led_state= !led_state;
      gpio_set_level(led,led_state);
      if(led_state){
           
            printf("LED BAT\n");
        } else {
           
            printf("LED TAT\n");
        }
    }
        lastButton_state=current_state;
    
        vTaskDelay(500 /portTICK_PERIOD_MS);
     }
    }



