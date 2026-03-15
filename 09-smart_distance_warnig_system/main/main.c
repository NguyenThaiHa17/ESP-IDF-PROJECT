#include<stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "lcd_i2c.h"
#include "h04.h"
#include "system_event.h"
#include "control.h"
QueueHandle_t myqueue;
void app_main(){
printf("Dang kiem tra LCD...\n");

    // Khởi tạo LCD
    lcd_init();
    lcd_clear();

    lcd_put_cur(0, 0);
    lcd_send_string("Hello Ha!");

    lcd_put_cur(1, 0);
    lcd_send_string("Dang khoi tao...");
vTaskDelay(pdMS_TO_TICKS(2000));
    myqueue=xQueueCreate(10,sizeof(system_event_t));
    xTaskCreate(h04_task,"H04",4096,NULL,5,NULL);
    xTaskCreate(control_task,"CONTROL",4096,NULL,5,NULL);

}