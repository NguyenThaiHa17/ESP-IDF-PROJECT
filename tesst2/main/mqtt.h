#pragma once

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"


#define BROKER_URI "mqtts://706bc88c8d8c46478ee00d2bd401516c.s1.eu.hivemq.cloud:8883"


#define MQTT_USERNAME "nth281"
#define MQTT_PASSWORD "Nguyenthaiha281@"

#define MQTT_TOPIC_PUB "nth281/led/status"
#define MQTT_TOPIC_SUB "nth281/led/control"

#define MQTT_TAG "MQTT"

void mqtt_app_start(void);
void mqtt_app_publish(const char *topic, const char *data, int len);