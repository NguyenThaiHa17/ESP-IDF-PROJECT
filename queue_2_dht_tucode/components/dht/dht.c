#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"

#define DHT_TIMEOUT 1000

static int wait_for_level(gpio_num_t pin, int level, int timeout_us)
{
    int64_t start = esp_timer_get_time();

    while (gpio_get_level(pin) == level)
    {
        if ((esp_timer_get_time() - start) > timeout_us)
            return -1;
    }
    return 0;
}

esp_err_t dht_read_float_data(dht_type_t type,
                              gpio_num_t pin,
                              float *humidity,
                              float *temperature)
{
    uint8_t data[5] = {0};

    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(20));   // start signal
    gpio_set_level(pin, 1);
    ets_delay_us(30);

    gpio_set_direction(pin, GPIO_MODE_INPUT);

    if (wait_for_level(pin, 1, DHT_TIMEOUT) < 0) return ESP_FAIL;
    if (wait_for_level(pin, 0, DHT_TIMEOUT) < 0) return ESP_FAIL;
    if (wait_for_level(pin, 1, DHT_TIMEOUT) < 0) return ESP_FAIL;

    for (int i = 0; i < 40; i++)
    {
        if (wait_for_level(pin, 0, DHT_TIMEOUT) < 0) return ESP_FAIL;

        int64_t start = esp_timer_get_time();

        if (wait_for_level(pin, 1, DHT_TIMEOUT) < 0) return ESP_FAIL;

        if ((esp_timer_get_time() - start) > 40)
            data[i / 8] |= (1 << (7 - (i % 8)));
    }

    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
        return ESP_FAIL;

    if (type == DHT_TYPE_DHT11)
    {
        *humidity = data[0];
        *temperature = data[2];
    }
    else
    {
        *humidity = ((data[0] << 8) | data[1]) * 0.1;
        *temperature = (((data[2] & 0x7F) << 8) | data[3]) * 0.1;
        if (data[2] & 0x80) *temperature *= -1;
    }

    return ESP_OK;
}