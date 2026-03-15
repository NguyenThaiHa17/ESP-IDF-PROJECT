#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "lcd_i2c.h"
#include "system_event.h"
#include "control.h"
#include "driver/gpio.h"

extern QueueHandle_t myqueue;
#define LED_PIN GPIO_NUM_2
void control_task(void*pvParameters){
    system_event_t event;
    char lcd_string[16];
    gpio_set_direction (LED_PIN,GPIO_MODE_OUTPUT);

    lcd_init();
    vTaskDelay((pdTICKS_TO_MS(100)));
    lcd_clear();
    while (1)
    {
        if(xQueueReceive(myqueue,&event,portMAX_DELAY)){
            float d= event.distance_cm;
           
             lcd_put_cur(0, 0);
            sprintf(lcd_string, "Dist: %.2f cm  ", d); // Thêm khoảng trắng để xóa số cũ
            lcd_send_string(lcd_string);
            lcd_put_cur(1,0);
            if(d<20){
                gpio_set_level(LED_PIN,1);
                lcd_send_string("!!! DANGER !!! ");
                printf ("Dist: %.2f cm -> DANGER\n",d);

            }
            else if( d<50){
                gpio_set_level(LED_PIN,0);
                lcd_send_string("WARNING        ");
                printf ("Dist: %.2f cm -> WARNING\n",d);

            }
            else {
                gpio_set_level(LED_PIN,0);
                lcd_send_string("SAFE           ");
                printf("Dist: %2.f cm -> SAFE\n",d);
            }
        }
        /* code */
    }
    
}