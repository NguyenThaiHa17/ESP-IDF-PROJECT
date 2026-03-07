#include "dht.h"
#include "dht_task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "system_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define DHT_PIN GPIO_NUM_4
#define DHT_TYPE DHT_TYPE_DHT11

extern QueueHandle_t systemQueue;
void dht_task(void*pvParameters){
    system_event_t event;
    int fail_count=0;
    while (1)
    {
        float temperature=0;
        float humidity=0;
        if(dht_read_float_data(DHT_TYPE_DHT11,DHT_PIN,&humidity,&temperature)==ESP_OK){
            ESP_LOGI("DHT", "Doc thanh cong: Temp: %.1f°C, Hum: %.1f%%", temperature, humidity);
            event.type=EVENT_TEMP;
            event.temperature=temperature;
            xQueueSend(systemQueue,&event,portMAX_DELAY);
            fail_count=0;

        }
        else{
            fail_count++;
            ESP_LOGI("DHT","READ FAIL : %d",fail_count);
            if(fail_count >=5){
                ESP_LOGI("DHT","SENSOR-SAFE");
                fail_count=0;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
        
    }
    
}
