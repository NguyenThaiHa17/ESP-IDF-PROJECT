#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/* Task function */
void vTask1(void *pvParameters)
{
    while (1)
    {
        printf("TASK 1 running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));   // delay 1 giây
    }
}

/* app_main is also a FreeRTOS task */
void app_main(void)
{
    xTaskCreate(
        vTask1,          // Task function
        "Task1",         // Name
        2048,            // Stack size (words)
        NULL,            // Parameter
        5,               // Priority
        NULL             // Task handle
    );
}
