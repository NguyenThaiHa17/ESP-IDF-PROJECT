#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define TIME_LIMIT 1000
#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_18

static const char* TAG = "MYBUTTON";

typedef struct {
    int level;
    TickType_t tick_time;
} button_event_t;

QueueHandle_t myQueue;
static TickType_t last_time=0;
void IRAM_ATTR button_isr_handler(void* arg) {
    TickType_t now=xTaskGetTickCountFromISR();
    if((now-last_time)<pdMS_TO_TICKS(50) ){
        return;
    }
    last_time=now;
    button_event_t event;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    event.level = gpio_get_level(BUTTON_PIN);
    event.tick_time = xTaskGetTickCountFromISR();

    xQueueSendFromISR(myQueue, &event, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void button_control(void* pvParameters) {
    button_event_t event;
    TickType_t press_time = 0;

    while (1) {
        if (xQueueReceive(myQueue, &event, portMAX_DELAY)) {
            if (event.level == 0) { // Khi nhấn nút (Pull-up kéo xuống 0)
                press_time = event.tick_time;
            } 
            else if (event.level == 1) { // Khi thả nút (Sửa lỗi else ở đây)
                TickType_t current_time = event.tick_time;
                TickType_t duration_time = current_time - press_time;
                int duration_ms = duration_time * portTICK_PERIOD_MS;

                if (duration_ms > TIME_LIMIT) {
                    ESP_LOGI(TAG, "LONG_PRESS");
                    for (int i = 0; i < 3; i++) {
                        gpio_set_level(LED_PIN, 1);
                        vTaskDelay(pdMS_TO_TICKS(500));
                        gpio_set_level(LED_PIN, 0);
                        vTaskDelay(pdMS_TO_TICKS(500));
                    }
                } 
                else {
                    ESP_LOGI(TAG, "SHORT_PRESS");
                    gpio_set_level(LED_PIN, 1);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    gpio_set_level(LED_PIN, 0);
                }
                press_time = 0; // Reset sau khi đã tính toán xong
            }
        }
    }
}

void app_main(void) {
    myQueue = xQueueCreate(10, sizeof(button_event_t));
    
    gpio_reset_pin(LED_PIN); // Đảm bảo pin sạch trước khi dùng
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE, // Ngắt ở cả 2 cạnh để tính thời gian
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);
    
    xTaskCreate(button_control, "BUTTON", 4096, NULL, 5, NULL);
}