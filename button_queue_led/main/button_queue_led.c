#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char*TAG="BUTTON EVENT";

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_18
QueueHandle_t myQueue;
void IRAM_ATTR button_isr_handler(void*arg){
    static uint32_t last_time=0;
    uint32_t current_time=xTaskGetTickCountFromISR();
    if(current_time- last_time >pdMS_TO_TICKS(200)){
        last_time=current_time;
    
    int btn=(int) arg;
    BaseType_t xHigherPriorityTaskWoken=pdFALSE;
    xQueueSendFromISR(myQueue,&btn,&xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken){
        portYIELD_FROM_ISR();
    }
}
}
void countter_task(void*pvParameters){
    int count=0;
    int btn;
    while (1)
    {
        if(xQueueReceive(myQueue,&btn,portMAX_DELAY)){
            count++;
  ESP_LOGI(TAG, "Button pressed: %d", count);
        }
        if(count%5==0){
            ESP_LOGW(TAG, "5 presses! LED ON");


            gpio_set_level(LED_PIN,1);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_set_level(LED_PIN,0);
        }
        /* code */
    }
    

}
void app_main(void)
{
    myQueue = xQueueCreate(10,sizeof(int));
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
    gpio_config_t io_conf={
        .pin_bit_mask=(1ULL << BUTTON_PIN),
        .mode=GPIO_MODE_INPUT,
        .pull_up_en=GPIO_PULLUP_ENABLE,
        .intr_type=GPIO_INTR_NEGEDGE,
        

    };
    gpio_config (&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN,button_isr_handler,(void*)BUTTON_PIN);
    xTaskCreate(countter_task,"COUNTER",4096,NULL,6,NULL);


}
