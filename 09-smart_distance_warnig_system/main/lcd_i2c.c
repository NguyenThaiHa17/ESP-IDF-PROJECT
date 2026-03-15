#include "lcd_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <string.h>
#include "esp_rom_sys.h" // Thư viện chứa hàm esp_rom_delay_us
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04
#define RS 0x01

static void lcd_send_cmd(char cmd);
static void lcd_send_data(char data);

static void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}
// Sửa lại hàm này để chỉ gửi 1 gói I2C duy nhất
static void lcd_write_byte(uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, data | LCD_BACKLIGHT, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

// Sửa lại hàm này để tạo xung Enable chuẩn
static void lcd_toggle_enable(uint8_t data)
{
    // Bước 1: Kéo chân Enable lên cao để chuẩn bị gửi
    lcd_write_byte(data | ENABLE);
    esp_rom_delay_us(500); // Dùng hàm mới như trình biên dịch gợi ý
    
    // Bước 2: Kéo chân Enable xuống thấp để "chốt" dữ liệu vào bộ nhớ LCD
    lcd_write_byte(data & ~ENABLE);
    esp_rom_delay_us(500);
}

// Cập nhật lại hàm lcd_send để nó gọn hơn
static void lcd_send(uint8_t data, uint8_t mode)
{
    uint8_t high = data & 0xF0;
    uint8_t low = (data << 4) & 0xF0;

    // Chỉ gọi toggle_enable, không gọi write_byte dư thừa ở đây
    lcd_toggle_enable(high | mode);
    lcd_toggle_enable(low | mode);
}

static void lcd_send_cmd(char cmd)
{
    lcd_send(cmd, 0);
}

static void lcd_send_data(char data)
{
    lcd_send(data, RS);
}

void lcd_clear()
{
    lcd_send_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void lcd_put_cur(int row, int col)
{
    int pos;

    if (row == 0) pos = 0x80 + col;
    else pos = 0xC0 + col;

    lcd_send_cmd(pos);
}

void lcd_send_string(char *str)
{
    while (*str)
    {
        lcd_send_data(*str++);
    }
}

void lcd_init()
{
    i2c_master_init();

    vTaskDelay(pdMS_TO_TICKS(50));

    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_clear();
}