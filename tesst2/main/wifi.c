#include "wifi.h"
#include "mqtt.h"
static const char *TAG = "WIFI";

static int s_retry_num=0;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if(event_base ==WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        esp_wifi_connect();

    } else if(event_base ==WIFI_EVENT &&   event_id == WIFI_EVENT_STA_DISCONNECTED){
        if(s_retry_num < MAXIMUM_RETRY){
            esp_wifi_connect();
            s_retry_num ++;
ESP_LOGI(TAG,"THU KET NOI LAI");
        }
         else {
            ESP_LOGI(TAG,"KET NOI THAT BAI SAU %d lan thu ",MAXIMUM_RETRY);
         }
    }
    else if (event_base== IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t* event= (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Đã lấy được IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num=0;

          mqtt_app_start();
    }
}
void app_wifi_init(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));


    wifi_config_t wifi_config ={ 
        .sta={
            .ssid=ESP_WIFI_SSID,
            .password=ESP_WIFI_PASS,
            .threshold.authmode=WIFI_AUTH_WPA2_PSK,
        },
    };
ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Hoàn tất khởi tạo WiFi ở chế độ Station.");
}
