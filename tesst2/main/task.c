#include "task.h"
#include "wifi.h"
#include "mqtt.h"

static QueueHandle_t my_gpio_led=NULL;
static int led_current=0;

static void IRAM_ATTR gpio_isr_handler(void* arg){
    uint32_t gpio_num = (uint32_t) arg;
     xQueueSendFromISR(my_gpio_led,&gpio_num,NULL);
}
    void button_task(void* arg){
        uint32_t io_num;

    while (1)
    {
        if(xQueueReceive(my_gpio_led,&io_num,portMAX_DELAY)){
            vTaskDelay(pdMS_TO_TICKS(200));
            if(gpio_get_level(io_num)==0){
                led_current=!led_current;
                led_level(led_current);

                const char* msg = led_current ? "1" : "0";
                mqtt_app_publish(MQTT_TOPIC_PUB, msg, strlen(msg));
            }
        }
        /* code */
    }
    
    }
void task_init(void){
        my_gpio_led =xQueueCreate(10,sizeof(uint32_t));

        gpio_reset_pin(LED_GPIO);
        gpio_set_direction(LED_GPIO,GPIO_MODE_OUTPUT);

        gpio_config_t button ={
            .pin_bit_mask=(1ULL << BUTTON_PIN),
            .mode=GPIO_MODE_INPUT,
            .pull_up_en=1,
        .intr_type = GPIO_INTR_NEGEDGE
        };
        gpio_config(&button);

        gpio_install_isr_service(0);
        gpio_isr_handler_add(BUTTON_PIN,gpio_isr_handler,(void*)BUTTON_PIN);


}
void led_level(int level){
    led_current=level;
    gpio_set_level(LED_GPIO,level);
    
}
void button_start_task(void){
    xTaskCreate(button_task,"BUTTON",4096,NULL,10,NULL);
}

    
