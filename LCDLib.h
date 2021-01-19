#include <F28x_Project.h>
#include <interrupt.h>

#define LCD_WIDTH 320
#define LCD_HEIGHT 480


//40 = LCD CS
//41 = CMD/DATA
//59 = RESET
//52 = LCD Write
//D[11:0] = Gpio[11:0]
//D[15:12] = Gpio[125:122]

//uint16_t draw_color(float value);

void init_lcd_gpio(void);

void lcd_reg(uint16_t address);

void lcd_data(uint16_t data);

void inline load_bus(uint16_t data);

void inline write_strobe(void);

void init_lcd_regs(void);

void draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void draw_point(uint16_t x, uint16_t y, uint16_t color);

void vertical_scroll(uint16_t address);

