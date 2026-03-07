#include <stdio.h>
#include "freertos/FreeRTOS.h" // Luôn để cái này lên đầu nhóm FreeRTOS
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "system_event.h"
#include "control.h"
extern QueueHandle_t systemQueue;
#define LED_PIN GPIO_NUM_2
#define BUZZER_PIN GPIO_NUM_19
void control_task(void *pvParameters)
{
    system_event_t event;
    int warning_active = 0;
    TickType_t mute_until = 0;   // thời điểm hết mute

    while (1)
    {
        if (xQueueReceive(systemQueue, &event, portMAX_DELAY))
        {
            switch (event.type)
            {
            case EVENT_TEMP:

                if (event.temperature > 26)
                {
                    // nếu chưa hết thời gian mute
                    if (xTaskGetTickCount() >= mute_until)
                    {
                        warning_active = 1;
                    }
                }
                else
                {
                    warning_active = 0;  // nhiệt độ an toàn
                }

                break;

            case EVENT_BUTTON_LONG:

                warning_active = 0;
                mute_until = xTaskGetTickCount() + pdMS_TO_TICKS(5000); // tắt 10s
                break;

            default:
                break;
            }

            gpio_set_level(LED_PIN, warning_active);
            gpio_set_level(BUZZER_PIN, warning_active);
        }
    }
}