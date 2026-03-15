#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "driver/i2c.h"

#define LCD_ADDR 0x27

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

void lcd_init();
void lcd_clear();
void lcd_put_cur(int row, int col);
void lcd_send_string(char *str);

#endif