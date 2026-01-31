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
#define led 2
#define button 5
bool ledState=false;
esp_err_t get_handler(httpd_req_t *req){
    ledState =! ledState;
    gpio_set_level(led,ledState);
const char* resp = "<html><body><h1>ESP32 Switch</h1>"
                       "<p><a href='/'><button style='height:50px;width:100px'>ON/OFF</button></a></p>"
                       "</body></html>";
    httpd_resp_send(req,resp,HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
void start_webserver(){
      httpd_handle_t server = NULL;
      httpd_config_t config= HTTPD_DEFAULT_CONFIG();
      if(httpd_start(&server,&config)==ESP_OK){
        httpd_uri_t uri_get = { .uri = "/", .method = HTTP_GET, .handler = get_handler };
        httpd_register_uri_handler(server,&uri_get);
}
}
static void wifi_event_handler(void*arg,esp_event_base_t event_base ,int32_t event_id,void*event_data)
{
    if(event_id==WIFI_EVENT_STA_START){
        esp_wifi_connect();

    } else if(event_id==WIFI_EVENT_STA_DISCONNECTED){
        esp_wifi_connect();
        printf("Dang ket noi....");

    }
    else if(event_base==IP_EVENT && event_id==IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t*event=(ip_event_got_ip_t*)event_data;
        printf("Da Ket Noi IP:"IPSTR "\n",IP2STR(&event->ip_info.ip));
        start_webserver();

    }
}

void app_main(void)
{
   esp_err_t ret = nvs_flash_init();
    
    if(ret==ESP_ERR_NVS_NO_FREE_PAGES || ret==ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());

    }
    ESP_ERROR_CHECK(ret);
    gpio_reset_pin(led);
    gpio_reset_pin(button);
    gpio_set_direction(led,GPIO_MODE_OUTPUT);
    gpio_set_direction(button,GPIO_MODE_INPUT);
    gpio_pullup_en(button);
   
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg= WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config={
        .sta={
            .ssid=WIFI_SSID,
            .password=WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA,&wifi_config);
    esp_wifi_start();    

    while(1){ 
        if(gpio_get_level(button)== 0 ){
        ledState=! ledState;
        gpio_set_level(led,ledState);
        vTaskDelay(300/portTICK_PERIOD_MS);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    }
}
    