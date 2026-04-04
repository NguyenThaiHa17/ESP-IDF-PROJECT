#include "mqtt.h"

#include "esp_crt_bundle.h"


#define LED_GPIO 2  

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t client = NULL;


void led_init() {
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

void led_set(int state) {
    gpio_set_level(LED_GPIO, state);
}

void mqtt_app_publish(const char *topic, const char *data, int len) {
    if (client != NULL) {
        esp_mqtt_client_publish(client, topic, data, len, 1, 0);
    }
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {

       case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "MQTT CONNECTED");
    esp_mqtt_client_subscribe(client, "nth281/led/control", 0);
    break;
       case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "DATA: %.*s", event->data_len, event->data);

    // Kiểm tra đúng topic
    if (strncmp(event->topic, "nth281/led/control", event->topic_len) == 0) {
        
        // CÁCH SỬA MỚI: Chỉ kiểm tra ký tự đầu tiên
        if (event->data[0] == '1') {
            led_set(1);
            ESP_LOGI(TAG, "LED ON");
        } 
        else if (event->data[0] == '0') {
            led_set(0);
            ESP_LOGI(TAG, "LED OFF");
        }
    }
    break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT ERROR");
            break;

        default:
            break;
    }
}


void mqtt_app_start() {
    led_init();
esp_mqtt_client_config_t mqtt_cfg = {
  
        .broker.address.uri = "mqtts://706bc88c8d8c46478ee00d2bd401516c.s1.eu.hivemq.cloud:8883",
    .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
        .credentials.username = "nth281",
        .credentials.authentication.password = "Nguyenthaiha281@",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}