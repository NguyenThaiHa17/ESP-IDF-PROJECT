#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
// Project Headers - THIẾU CÁI NÀY LÀ BỊ LỖI NGAY
#include "button.h"
#include "control.h"
#include "dht_task.h"
#include "system_event.h"
QueueHandle_t systemQueue;
#define LED_PIN GPIO_NUM_2
#define BUZZER_PIN GPIO_NUM_19

void app_main(void)
{
    systemQueue=xQueueCreate(10,sizeof(system_event_t));
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER_PIN,GPIO_MODE_OUTPUT);

    button_init();
    xTaskCreate(dht_task,"DHT",4096,NULL,5,NULL);
    xTaskCreate(control_task,"CONTROL",4096,NULL,5,NULL);
    xTaskCreate(button_task,"BUTTON",4096,NULL,10,NULL);
}
