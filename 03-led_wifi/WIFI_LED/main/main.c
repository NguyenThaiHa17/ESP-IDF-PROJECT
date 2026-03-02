#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#define WIFI_SSID "Dream Home"
#define WIFI_PASS "hang@19@19"
#define led GPIO_NUM_2
#define button GPIO_NUM_5
bool ledState=false;
esp_err_t get_handler (httpd_req_t *req){
    ledState=!ledState;
    gpio_set_level(led,ledState);
    const char*resp="<html><body><h1>ESP32 Switch</h1>"
                       "<p><a href='/'><button style='height:50px;width:100px'>ON/OFF</button></a></p>"
                       "</body></html>";
                       httpd_resp_send(req,resp,HTTPD_RESP_USE_STRLEN);
                       return ESP_OK;
}
void start_webserver(){
    httpd_handle_t server= NULL;
    httpd_config_t config=HTTPD_DEFAULT_CONFIG();
    if(httpd_start(&server,&config)==ESP_OK){
        httpd_uri_t uri_get=  { .uri = "/", .method = HTTP_GET, .handler = get_handler };
        httpd_register_uri_handler(server,&uri_get);
        
    }
} 
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
      
            esp_wifi_connect();
         printf("DANG KET NOI");
    
        
  
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf( "DA KET NOI IP:" IPSTR "\n", IP2STR(&event->ip_info.ip));
      start_webserver();
    }
}

void app_main(void){
    esp_err_t ret=nvs_flash_init();
    if(ret==ESP_ERR_NVS_NO_FREE_PAGES || ret==ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
          ESP_ERROR_CHECK(ret);
        gpio_config_t io_conf={};
        io_conf.pin_bit_mask=(1ULL<<led);
        io_conf.mode=GPIO_MODE_OUTPUT;
        io_conf.pull_up_en =0;
        io_conf.pull_down_en=0;
        io_conf.intr_type=GPIO_INTR_DISABLE;
        gpio_config(&io_conf);

        io_conf.pin_bit_mask=(1ULL<<button);
        io_conf.mode=GPIO_MODE_INPUT;
        io_conf.pull_up_en=1;
        gpio_config(&io_conf);
             // Áp dụng cấu hình cho Nút nhấn
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg=WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&event_handler,NULL,NULL);
    esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&event_handler,NULL,NULL);

    wifi_config_t wifi_config={
        .sta={
            .ssid=WIFI_SSID,
            .password=WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA,&wifi_config);
    esp_wifi_start();
    while(1)
{
    if(gpio_get_level(button)==0){
        ledState= !ledState;
        gpio_set_level(led,ledState);
        vTaskDelay(300/portTICK_PERIOD_MS);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
}
}
