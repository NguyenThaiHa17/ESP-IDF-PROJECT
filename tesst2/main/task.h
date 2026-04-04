#pragma once 

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define BUTTON_PIN GPIO_NUM_5
#define LED_GPIO   GPIO_NUM_2        
void task_init(void);
void led_level(int level);
void button_start_task(void);
void button_task(void* arg);

