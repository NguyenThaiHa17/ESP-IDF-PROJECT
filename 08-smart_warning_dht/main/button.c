#include "button.h"
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "system_event.h"
#define BUTTON_PIN GPIO_NUM_18
static QueueHandle_t buttonQueue;
extern QueueHandle_t systemQueue;

void IRAM_ATTR button_irs(void*arg){
    uint32_t gpio_num=(int32_t)arg;
    xQueueSendFromISR(buttonQueue,&gpio_num,NULL);

}
void button_task(void*arg){
    uint32_t io_num;
    TickType_t press_time=0;
    while (1)
    {
        if(xQueueReceive(buttonQueue,&io_num,portMAX_DELAY)){
            if(gpio_get_level(io_num)==0){
                press_time=xTaskGetTickCount();

            }
            else {
                TickType_t duration=xTaskGetTickCount()-press_time;
                if(duration > pdMS_TO_TICKS(3000)){
                    system_event_t event;
                    event.type=EVENT_BUTTON_LONG;
                    xQueueSend(systemQueue,&event,portMAX_DELAY);

                }
            }
        }
        /* code */
    }
    
}

void button_init(void){
    buttonQueue = xQueueCreate(10, sizeof(uint32_t));

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_irs, (void*) BUTTON_PIN);

}