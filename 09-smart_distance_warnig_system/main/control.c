#include <stdio.h>
#include "control.h"
#include "lcd_i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "system_event.h"

#define LED_GPIO GPIO_NUM_2

extern QueueHandle_t systemQueue;

void control_task(void *pvParameters)
{
    system_event_t event;

    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    lcd_init();
    lcd_clear();

    while (1)
    {
        if (xQueueReceive(systemQueue, &event, portMAX_DELAY))
        {
            float d = event.distance_cm;

            lcd_clear();
            lcd_put_cur(0,0);

            if (d < 20)
            {
                gpio_set_level(LED_GPIO, 1);

                printf("Distance: %.2f cm -> DANGER\n", d);

                lcd_send_string("DANGER!");
                lcd_put_cur(1,0);
                lcd_send_string("Too Close");
            }

            else if (d < 50)
            {
                gpio_set_level(LED_GPIO, 0);

                printf("Distance: %.2f cm -> WARNING\n", d);

                lcd_send_string("WARNING");
                lcd_put_cur(1,0);
                lcd_send_string("Be Careful");
            }

            else
            {
                gpio_set_level(LED_GPIO, 0);

                printf("Distance: %.2f cm -> SAFE\n", d);

                lcd_send_string("SAFE");
                lcd_put_cur(1,0);
                lcd_send_string("Distance OK");
            }
        }
    }
}