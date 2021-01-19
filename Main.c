 #include <F28x_Project.h>
#include <interrupt.h>
#include <AIC23.h>
#include <math.h>
#include "sram.h"
#include "LCDlib.h"

#include "dsp.h"
#include "fpu32/fpu_rfft.h"


void init_dma(void);
void fft_init(void);
void init_rx_int(void);
void init_adc(void);
void init_gpios(void);
void printstart(void);
void dft(void);

volatile int16_t twofiddy = 0;
volatile int16_t flag = 0;
volatile int16_t index = 0;
float Xreal[256];
float Ximag[256];
float DFTMags[256];
volatile int16_t ping[512];
volatile int16_t pong[512];
volatile int16_t pongflag = 0;

float mag;
float number;

//************* Defines *****************//

#define BURST 1
#define TRANSFER 255
#define TEST_SIZE       (1024U)
#define FFT_STAGES      (10U)
#define FFT_SIZE        (1 << FFT_STAGES)

RFFT_F32_STRUCT rfft;
RFFT_F32_STRUCT_Handle hnd_rfft = &rfft;
volatile int16 * ptr;


#pragma DATA_SECTION(inputbuffer, "input_buff")
volatile float inputbuffer[256];

#pragma DATA_SECTION(outputbuffer, "output_buff")
volatile float outputbuffer[256];

#pragma DATA_SECTION(magbuffer, "mag_buff")
volatile float magbuffer[256];

#pragma DATA_SECTION(hanning, "hanning")
float hanning[256] = { 0.0000, 0.0002, 0.0006, 0.0014, 0.0024, 0.0038, 0.0055, 0.0074, 0.0097, 0.0122, 0.0151,
                       0.0183, 0.0217, 0.0254, 0.0295, 0.0338, 0.0384, 0.0432, 0.0484, 0.0538, 0.0595, 0.0655,
                       0.0717, 0.0782, 0.0849, 0.0919, 0.0991, 0.1066, 0.1144, 0.1223, 0.1305, 0.1389, 0.1475,
                       0.1564, 0.1654, 0.1747, 0.1841, 0.1938, 0.2036, 0.2136, 0.2238, 0.2342, 0.2447, 0.2554,
                       0.2662, 0.2771, 0.2882, 0.2994, 0.3108, 0.3223, 0.3338, 0.3455, 0.3573, 0.3691, 0.3810,
                       0.3930, 0.4051, 0.4172, 0.4294, 0.4416, 0.4539, 0.4661, 0.4784, 0.4908, 0.5031, 0.5154,
                       0.5277, 0.5400, 0.5523, 0.5645, 0.5767, 0.5888, 0.6009, 0.6130, 0.6249, 0.6368, 0.6486,
                       0.6604, 0.6720, 0.6835, 0.6949, 0.7062, 0.7173, 0.7284, 0.7393, 0.7500, 0.7606, 0.7710,
                       0.7813, 0.7914, 0.8013, 0.8111, 0.8206, 0.8300, 0.8391, 0.8481, 0.8568, 0.8653, 0.8736,
                       0.8817, 0.8895, 0.8971, 0.9045, 0.9116, 0.9185, 0.9251, 0.9315, 0.9376, 0.9434, 0.9489,
                       0.9542, 0.9592, 0.9640, 0.9684, 0.9726, 0.9765, 0.9801, 0.9834, 0.9864, 0.9891, 0.9915,
                       0.9936, 0.9954, 0.9969, 0.9981, 0.9991, 0.9997, 1.0000, 1.0000, 0.9997, 0.9991, 0.9981,
                       0.9969, 0.9954, 0.9936, 0.9915, 0.9891, 0.9864, 0.9834, 0.9801, 0.9765, 0.9726, 0.9684,
                       0.9640, 0.9592, 0.9542, 0.9489, 0.9434, 0.9376, 0.9315, 0.9251, 0.9185, 0.9116, 0.9045,
                       0.8971, 0.8895, 0.8817, 0.8736, 0.8653, 0.8568, 0.8481, 0.8391, 0.8300, 0.8206, 0.8111,
                       0.8013, 0.7914, 0.7813, 0.7710, 0.7606, 0.7500, 0.7393, 0.7284, 0.7173, 0.7062, 0.6949,
                       0.6835, 0.6720, 0.6604, 0.6486, 0.6368, 0.6249, 0.6130, 0.6009, 0.5888, 0.5767, 0.5645,
                       0.5523, 0.5400, 0.5277, 0.5154, 0.5031, 0.4908, 0.4784, 0.4661, 0.4539, 0.4416, 0.4294,
                       0.4172, 0.4051, 0.3930, 0.3810, 0.3691, 0.3573, 0.3455, 0.3338, 0.3223, 0.3108, 0.2994,
                       0.2882, 0.2771, 0.2662, 0.2554, 0.2447, 0.2342, 0.2238, 0.2136, 0.2036, 0.1938, 0.1841,
                       0.1747, 0.1654, 0.1564, 0.1475, 0.1389, 0.1305, 0.1223, 0.1144, 0.1066, 0.0991, 0.0919,
                       0.0849, 0.0782, 0.0717, 0.0655, 0.0595, 0.0538, 0.0484, 0.0432, 0.0384, 0.0338, 0.0295,
                       0.0254, 0.0217, 0.0183, 0.0151, 0.0122, 0.0097, 0.0074, 0.0055, 0.0038, 0.0024, 0.0014,
                       0.0006, 0.0002, 0.0000
                     };

volatile int16 *DMA_CH6_Dest;
volatile int16 *DMA_CH6_Source;
volatile int16 *DMA_CH5_Dest;
volatile int16 *DMA_CH5_Source;
__interrupt void local_D_INTCH6_ISR(void);
__interrupt void local_D_INTCH5_ISR(void);
uint16_t draw_color(float value);

#define pi 3.141592653

int main(void)
{
    EALLOW;

    InitSysCtrl();

    init_lcd_gpio();
    init_lcd_regs();

    InitSPIA();
    InitMcBSPb();
    InitBigBangedCodecSPI();
    InitAIC23();
    init_dma();
    fft_init();

    bool increasing = true;
    uint16_t scrollptr = 0;
    uint16_t point = 0;
    uint16_t y;
    uint16_t clr;
    float val;
    while(1){
        if(flag){
            RFFT_f32_sincostable(hnd_rfft);
            for(int i = 0;i<512; i+=2){
                inputbuffer[i/2] = (float)(((int32)ptr[i] + (int32)ptr[i+1])/2)*hanning[i/2];
            }
            for(int i = 0; i<256 ; i++){
                outputbuffer[i] = 0.0;
            }
            for(int i = 0; i<256; i++){
                magbuffer[i] = 0.0;
            }
            RFFT_f32(hnd_rfft);
            RFFT_f32_mag(hnd_rfft);

            vertical_scroll(scrollptr);
            y = scrollptr -1;
            if (scrollptr == 0) y = 479;
            for(int x = 0; x<320; x++){
                val = magbuffer[(int)(((float)x)*0.4)];
                clr = draw_color(val);
                draw_point(x, y, clr);
            }
            if(increasing) point++;
            else point--;
            if(point == 0 && !increasing) increasing = true;
            if(point == 319 && increasing) increasing = false;
            scrollptr++;
            if (scrollptr > 479) scrollptr = 0;

            flag = 0;
        }
    }
}

void fft_init(void){
    RFFT_f32_setInputPtr(hnd_rfft, inputbuffer);
    RFFT_f32_setOutputPtr(hnd_rfft, outputbuffer);
    RFFT_f32_setMagnitudePtr(hnd_rfft, magbuffer);
    RFFT_f32_setTwiddlesPtr(hnd_rfft, RFFT_f32_twiddleFactors);
    RFFT_f32_setStages(hnd_rfft, (FFT_STAGES - 1U));
    RFFT_f32_setFFTSize(hnd_rfft, (FFT_SIZE >> 1));
}


void init_dma(void){
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.DMA_CH6_INT= &local_D_INTCH6_ISR;
    PieVectTable.DMA_CH5_INT= &local_D_INTCH5_ISR;
    EDIS;
    DMAInitialize();

    // source and destination pointers
    DMA_CH6_Source = (volatile int16 *)&McbspbRegs.DRR2.all;
    DMA_CH6_Dest = (volatile int16 *)ping;
    DMA_CH5_Source = (volatile int16 *)pong;
    DMA_CH5_Dest = (volatile int16 *)&McbspbRegs.DXR2.all;
    DMACH6AddrConfig(DMA_CH6_Dest,DMA_CH6_Source);
    DMACH6BurstConfig(BURST,1,1);
    DMACH6TransferConfig(TRANSFER,0,1);
    DMACH6ModeConfig(74,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_ENABLE);
    DMACH6WrapConfig(0, 0, 255, 0);

    DMACH5AddrConfig(DMA_CH5_Dest,DMA_CH5_Source);
    DMACH5BurstConfig(BURST,1,1);
    DMACH5TransferConfig(TRANSFER,1,0);
    DMACH5ModeConfig(74,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_ENABLE);
    DMACH5WrapConfig(255, 0, 0, 0);

    //something about a bandgap voltage -> stolen from TI code
    EALLOW;
    CpuSysRegs.SECMSEL.bit.PF2SEL = 1;
    EDIS;


    //interrupt enabling
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER7.bit.INTx6 = 1;   // Enable PIE Group 7, INT 2 (DMA CH2)
    PieCtrlRegs.PIEIER7.bit.INTx5 = 1;   // Enable PIE Group 7, INT 2 (DMA CH2)
    IER |= M_INT7;                         // Enable CPU INT6
    EINT;                                // Enable Global Interrupts

    EnableInterrupts();

    StartDMACH6();      // Start DMA channel -> stolen from TI code
    StartDMACH5();
}


//DMA interrupt -> modified from TI code
__interrupt void local_D_INTCH6_ISR(void)
{
    EALLOW;

    if(pongflag){
        DMA_CH6_Dest = (volatile int16 *)ping;
        DMA_CH5_Source = (volatile int16 *)pong;
        ptr = pong;
        flag = 1;
        pongflag = 0;
    }else{
        DMA_CH6_Dest = (volatile int16 *)pong;
        DMA_CH5_Source = (volatile int16 *)ping;
        ptr = ping;
        flag = 1;
        pongflag = 1;
    }

    DMACH6AddrConfig(DMA_CH6_Dest, DMA_CH6_Source);
    DMACH5AddrConfig(DMA_CH5_Dest, DMA_CH5_Source);

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP7; // ACK to receive more interrupts
                                            // from this PIE group
    EDIS;

    //restart DMA -> Dave added this :}
    StartDMACH6();

}

__interrupt void local_D_INTCH5_ISR(void)
{
    EALLOW;

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP7; // ACK to receive more interrupts
                                            // from this PIE group
    EDIS;

    //restart DMA -> Dave added this :}
    StartDMACH5();

}

uint16_t draw_color(float value){
    float percent = (value/30000.0)*100;
    if(value > 30000.0) percent = 100.0;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    red = 0x1F;
    blue = 0x00;
    if(percent < 50.0) red = (uint16_t)(33.0*(percent*2/100.0));
    if(blue > 50.0) blue = (uint16_t)(33.0*(percent - 50.0)*2/100.0);
    green = (uint16_t)(63.0*percent/100.0);
    uint16_t ret = (red<<11) | (green << 5) | blue;
    return ret;
}
