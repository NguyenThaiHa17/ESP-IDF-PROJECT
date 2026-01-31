#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <freertos/timers.h>
#include <freertos/task.h>

#define led 2
#define button 5
int led_state= 0;
int is_running = true;
void nhay_led(TimerHandle_t xTimer){
    led_state = !led_state;
    gpio_set_level(led,led_state);
    printf("Timer da reo! Den dang: %d\n",led_state);

} 
void app_main(void)
{
    gpio_reset_pin(led);
    gpio_reset_pin(button);
    gpio_set_direction(button,GPIO_MODE_INPUT);
    gpio_set_pull_mode(button,GPIO_PULLUP_ONLY);
    gpio_set_direction(led,GPIO_MODE_OUTPUT);

    TimerHandle_t myTimer = xTimerCreate("Timer_led",pdMS_TO_TICKS(500),pdTRUE,(void*)0,nhay_led);
    xTimerStart(myTimer,0);
    int lastButton=1;
    while (1)
    {
        int currentButton=gpio_get_level(button);
        if(lastButton==1 && currentButton==0){
            is_running= !is_running;
            if(is_running){
                xTimerStart(myTimer,0);
                printf("Da Nhan\n");
            }
            else {
                xTimerStop(myTimer,0);
                printf("Da Tat\n");
            }
        }
        lastButton=currentButton;
        vTaskDelay(pdMS_TO_TICKS(50));
        /* code */
    }
    
}
