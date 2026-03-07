#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "h04.h"
#include "control.h"
#include "system_event.h"

QueueHandle_t systemQueue;

void app_main(void)
{
    systemQueue = xQueueCreate(10, sizeof(system_event_t));

    xTaskCreate(h04_task, "h04_task", 4096, NULL, 5, NULL);
    xTaskCreate(control_task, "control_task", 4096, NULL, 5, NULL);
}