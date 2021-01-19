#include <F28x_Project.h>
#include <interrupt.h>
#include "LCDLib.h"

//40 = LCD CS
//41 = CMD/DATA
//59 = RESET
//52 = LCD Write
//D[11:0] = Gpio[11:0]
//D[15:12] = Gpio[125:122]

void init_lcd_gpio(void){
    EALLOW;
    GpioCtrlRegs.GPADIR.all |= 0x0FFF;//lower three nibbles
    GpioCtrlRegs.GPDDIR.all |= 0x3C000000;//upper nibble
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO40 = 1;//LCD CS
    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 1;//command/data pin
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;//write strobe
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;//LCD Reset

    GpioDataRegs.GPBDAT.bit.GPIO59 = 1;//set reset pin high
    GpioDataRegs.GPBDAT.bit.GPIO40 = 1;//lcd cs active
    GpioDataRegs.GPBDAT.bit.GPIO41 = 1;//command/data high
    GpioDataRegs.GPBDAT.bit.GPIO52 = 1;//write high
    GpioDataRegs.GPBDAT.bit.GPIO59 = 1;//LCD Reset
}


void lcd_reg(uint16_t address){
    GpioDataRegs.GPBDAT.bit.GPIO41 = 0;//set as command
    load_bus(address);
    write_strobe();
}


void lcd_data(uint16_t data){
    GpioDataRegs.GPBDAT.bit.GPIO41 = 1;//set as data
    load_bus(data);
    write_strobe();
}

void inline load_bus(uint16_t data){
    GpioDataRegs.GPADAT.all = (data & 0x0FFF);
    GpioDataRegs.GPDDAT.all = (data & 0xF000)>>2;
}

void inline write_strobe(void){
    GpioDataRegs.GPBDAT.bit.GPIO52 = 0;//write low
    GpioDataRegs.GPBDAT.bit.GPIO52 = 1;//write high
}

void init_lcd_regs(void){
    DELAY_US(100);
    GpioDataRegs.GPBDAT.bit.GPIO40 = 0;//lcd cs active
    GpioDataRegs.GPBDAT.bit.GPIO59 = 1;//LCD reset false
    DELAY_US(5000);
    GpioDataRegs.GPBDAT.bit.GPIO59 = 0;//LCD reset true
    DELAY_US(20);
    GpioDataRegs.GPBDAT.bit.GPIO59 = 1;//LCD reset false

    DELAY_US(65000);
    //fully clear screen

//    //power control registers
//    lcd_reg(0xC0);
//    lcd_data(0x19);
//    lcd_data(0x1A);
//
//    lcd_reg(0xC1);
//    lcd_data(0x45);
//    lcd_data(0x00);
//
//    lcd_reg(0xC2);
//    lcd_data(0x33);
//
//    //VCOM control
//    lcd_reg(0xC5);
//    lcd_data(0x00);
//    lcd_data(0x28);
//
//    //Fram rate control
//    lcd_reg(0xB1);
//    lcd_data(0xA0);
//    lcd_data(0x11);
//
//    //16 bit pixel mode
//    lcd_reg(0x3A);
//    lcd_data(0x55);
//
//    //Z inversion
//    lcd_reg(0xB4);
//    lcd_data(0x02);
//
//    //Display control function
//    lcd_reg(0xB6);
//    lcd_data(0x00);
//    lcd_data(0x42);
//    lcd_data(0x3B);
//
//    //Positive Gamma control
//    lcd_reg(0xE0);
//    lcd_data(0x1F);
//    lcd_data(0x25);
//    lcd_data(0x22);
//    lcd_data(0x0B);
//    lcd_data(0x06);
//    lcd_data(0x0A);
//    lcd_data(0x4E);
//    lcd_data(0xC6);
//    lcd_data(0x39);
//    lcd_data(0x00);
//    lcd_data(0x00);
//    lcd_data(0x00);
//    lcd_data(0x00);
//    lcd_data(0x00);
//    lcd_data(0x00);
//
//    //Negative Gamma control
//    lcd_reg(0xE1);
//    lcd_data(0x1F);
//    lcd_data(0x3F);
//    lcd_data(0x3F);
//    lcd_data(0x0F);
//    lcd_data(0x1F);
//    lcd_data(0x0F);
//    lcd_data(0x46);
//    lcd_data(0x49);
//    lcd_data(0x31);
//    lcd_data(0x05);
//    lcd_data(0x09);
//    lcd_data(0x03);
//    lcd_data(0x1C);
//    lcd_data(0x1A);
//    lcd_data(0x00);
//
//
//    lcd_reg(0xB6);
//    lcd_data(0x00);
//    lcd_data(0x22);
//
//    lcd_reg(0x36);
//    lcd_data(0x08);
//
//    //Sleep mode off
//    lcd_reg(0x11);
//    DELAY_US(50000);
//
//    draw_rectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, 0xFFFF);
//
//    lcd_reg(0x29);
        lcd_reg(0xF1);
        lcd_data(0x36);
        lcd_data(0x04);
        lcd_data(0x00);
        lcd_data(0x3C);
        lcd_data(0x0F);
        lcd_data(0x8F);


        lcd_reg(0xF2);
        lcd_data(0x18);
        lcd_data(0xA3);
        lcd_data(0x12);
        lcd_data(0x02);
        lcd_data(0xb2);
        lcd_data(0x12);
        lcd_data(0xFF);
        lcd_data(0x10);
        lcd_data(0x00);

        lcd_reg(0xF8);
        lcd_data(0x21);
        lcd_data(0x04);

        lcd_reg(0xF9);
        lcd_data(0x00);
        lcd_data(0x08);

        lcd_reg(0xC0);
        lcd_data(0x0f); //13
        lcd_data(0x0f); //10

        lcd_reg(0xC1);
        lcd_data(0x42); //43

        lcd_reg(0xC2);
        lcd_data(0x22);

        lcd_reg(0xC5);
        lcd_data(0x01); //00
        lcd_data(0x29); //4D
        lcd_data(0x80);

        lcd_reg(0xB6);
        lcd_data(0x00);
        lcd_data(0x02); //42
        lcd_data(0x3b);

        lcd_reg(0xB1);
        lcd_data(0xB0); //C0
        lcd_data(0x11);

        lcd_reg(0xB4);
        lcd_data(0x02); //01

        lcd_reg(0xE0);
        lcd_data(0x0F);
        lcd_data(0x18);
        lcd_data(0x15);
        lcd_data(0x09);
        lcd_data(0x0B);
        lcd_data(0x04);
        lcd_data(0x49);
        lcd_data(0x64);
        lcd_data(0x3D);
        lcd_data(0x08);
        lcd_data(0x15);
        lcd_data(0x06);
        lcd_data(0x12);
        lcd_data(0x07);
        lcd_data(0x00);

        lcd_reg(0xE1);
        lcd_data(0x0F);
        lcd_data(0x38);
        lcd_data(0x35);
        lcd_data(0x0a);
        lcd_data(0x0c);
        lcd_data(0x03);
        lcd_data(0x4A);
        lcd_data(0x42);
        lcd_data(0x36);
        lcd_data(0x04);
        lcd_data(0x0F);
        lcd_data(0x03);
        lcd_data(0x1F);
        lcd_data(0x1B);
        lcd_data(0x00);

        lcd_reg(0x20);                     // display inversion OFF

        lcd_reg(0x36);      // MEMORY_ACCESS_CONTROL (orientation stuff)
        lcd_data(0x48);

        lcd_reg(0x3A);      // COLMOD_PIXEL_FORMAT_SET
        lcd_data(0x55);     // 16 bit pixel

        lcd_reg(0x13); // Nomal Displaymode

        lcd_reg(0x11);                     // sleep out
        DELAY_US(150000);

        lcd_reg(0x29);                     // display on
}


void draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    uint16_t startX = x;
    uint16_t endX = x + w - 1;
    uint16_t startY = y;
    uint16_t endY = y + h -1;

    //set column bounds
    lcd_reg(0x2A);
    lcd_data(startX>>8);
    lcd_data(startX);
    lcd_data(endX>>8);
    lcd_data(endX);

    //set row bounds
    lcd_reg(0x2B);
    lcd_data(startY>>8);
    lcd_data(startY);
    lcd_data(endY>>8);
    lcd_data(endY);

    //write colors
    lcd_reg(0x2C);

    for(uint32_t i = 0; i<(uint32_t)w*(uint32_t)h; i++){
        lcd_data(color);
    }

}


void vertical_scroll(uint16_t address){
    //vertical scrolling definition
    lcd_reg(0x33);
    lcd_data(0x00);
    lcd_data(0x00);//top fixed area 0 lines
    lcd_data(480 >> 8);
    lcd_data(480 & 0xFF);//vertical scrolling area
    lcd_data(0x00);
    lcd_data(0x00);//bottom fixed area 0 lines

    //vertical scrolling start address
    lcd_reg(0x37);
    lcd_data(address>>8);
    lcd_data(address&0xff);
}

void draw_point(uint16_t x, uint16_t y, uint16_t color){

    //set column bounds
    lcd_reg(0x2A);
    lcd_data(x>>8);
    lcd_data(x);
    lcd_data(x>>8);
    lcd_data(x);

    //set row bounds
    lcd_reg(0x2B);
    lcd_data(y>>8);
    lcd_data(y);
    lcd_data(y>>8);
    lcd_data(y);

    //write color
    lcd_reg(0x2C);
    lcd_data(color);
}

//
//uint16_t draw_color(float value){
//    float percent = (value/100000.0)*100;
//    if(value > 100000.0) percent = 1.0;
//    uint16_t red;
//    uint16_t green;
//    uint16_t blue;
//    red = 0x1F;
//    blue = 0x00;
//    if(percent < 50.0) red = (uint16_t)(33.0*(percent*2/100.0));
//    if(blue > 50.0) blue = (uint16_t)(33.0*(percent - 50.0)*2/100.0);
//    green = (uint16_t)(63.0*percent/100.0);
//    uint16_t ret = (red<<11) | (green << 5) | blue;
//    ret = 0x1F << 11;
//    return ret;
//}
//

