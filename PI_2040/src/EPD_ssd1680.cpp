#include <stdint.h>
#include "SSD1680_hal.h"
#include "EPD.h"


/*
 white pixel RAM(BW) = 1 
 black pixel RAM(BW) = 0


VS[nX-LUTm] | Source | VCOM voltage |
____________|________|______________|
   00       | VSS    | DCVCOM       | none
   01       | VSH1   | DCVCOM+VSH1  | black color
   10       | VSL    | DCVCOM+VSL   | white color
   11       | VSH2   | N/A          | black ?
*/



//LUT for a fast partial refresh
static uint8_t ssd1680_lut_delta[SSD1680_LUT_SIZE] =
{
/*    Phase 0     Phase 1     Phase 2     Phase 3     Phase 4     Phase 5     Phase 6     Phase 7     Phase 8     Phase 9     Phase 10    Phase 11  */
/*VS  A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D   */
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut0 (wb == 0 && red == 0) */
    0b10101010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut1 (wb == 1 && red == 0) */
    0b01010101, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut2 (wb == 0 && red == 1) */
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut3 (wb == 1 && red == 1) */
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut4 ( ??? ) */
/*   TP[] - duration, SR[] - Repeats   */
/* TP[A] TP[B] SR[AB] TP[C] TP[D]  SR[CD] |	Repeats phase */
    60,   0,    0,     0,     0,    0,       0,     /* Phase 0  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 1  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 2  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 3  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 4  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 5  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 6  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 7  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 8  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 9  */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 10 */
     0,   0,    0,     0,     0,    0,       0,     /* Phase 11 */
 /*  Frame Rate (4 bit * 11 Phase = 6 bytes) */
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0x0, 0x0, 0x0, /*  Gate Scan Selection XON (3 bytes) */
};


static void EPD_SetWindows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    SSD1680_SendCommand(SSD1680_SET_RAM_X_POS); //0x44 SET_RAM_X_ADDRESS_START_END_POSITION
    SSD1680_SendData((x_start>>3) & 0xFF);
    SSD1680_SendData((x_end>>3) & 0xFF);
	
    SSD1680_SendCommand(SSD1680_SET_RAM_Y_POS); //0x45 SET_RAM_Y_ADDRESS_START_END_POSITION
    SSD1680_SendData(y_start & 0xFF);
    SSD1680_SendData((y_start >> 8) & 0xFF);
    SSD1680_SendData(y_end & 0xFF);
    SSD1680_SendData((y_end >> 8) & 0xFF);
}

static void EPD_SetCursor(uint16_t x, uint16_t y)
{
    SSD1680_SendCommand(SSD1680_SET_RAM_X_COUNT); //0x4E SET_RAM_X_ADDRESS_COUNTER
    SSD1680_SendData(x & 0xFF);

    SSD1680_SendCommand(SSD1680_SET_RAM_Y_COUNT); //0x4F SET_RAM_Y_ADDRESS_COUNTER
    SSD1680_SendData(y & 0xFF);
    SSD1680_SendData((y >> 8) & 0xFF);
}

void EPD_Load_BW(const uint8_t *bitImage)
{
    SSD1680_SendCommand(SSD1680_WRITE_BW_RAM); // 0x24
    SSD1680_SendData(bitImage, EPD_IMAGE_SIZE);
}

void EPD_Load_RED(const uint8_t *bitImage)
{
    SSD1680_SendCommand(SSD1680_WRITE_RED_RAM); // 0x26
    SSD1680_SendData(bitImage, EPD_IMAGE_SIZE);
}

void EPD_Update_Display(const uint8_t* lut)
{
    if(lut != (uint8_t*)0)
    {
        SSD1680_SendCommand(SSD1680_DISP_CTRL2);
// Display Update Sequence:
// 0b10000000 enable clock
// 0b01000000 enable analog
// 0b00100000 load temperature value
// 0b00010000 load default LUT
// 0b00001000 display mode 1 or 2
// 0b00000100 display
// 0b00000010 disable analog
// 0b00000001 disable clock    
        SSD1680_SendData(0xEC);
        SSD1680_SendCommand(SSD1680_WRITE_LUT); // 0x32
        SSD1680_SendData(lut, SSD1680_LUT_SIZE);
    }else{
        SSD1680_SendCommand(SSD1680_DISP_CTRL2); //0x22 Display Update Control
        SSD1680_SendData(0xF7);
    }
   	SSD1680_SendCommand(SSD1680_MASTER_ACTIVATE);// 0x20 Activate Display Update Sequence
}

void EPD_Init(bool halInit)
{
    if(halInit)
        SSD1680_Init();
	SSD1680_Reset();

	SSD1680_SendCommand(SSD1680_SW_RESET);//0x12

	SSD1680_SendCommand(SSD1680_DRIVER_CONTROL); //0x01 driver output control
	SSD1680_SendData((EPD_HEIGHT - 1) & 0xFF);
	SSD1680_SendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
	SSD1680_SendData(0x00);// GD = 0, SM = 0, TB = 0;

	SSD1680_SendCommand(SSD1680_DATA_MODE); //0x11 data entry mode
	SSD1680_SendData(0x03);//AM = 0, Y= 1 (increment), X = 1 (increment);

	EPD_SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
	EPD_SetCursor(0, 0);

    SSD1680_SendCommand(SSD1680_WRITE_VCOM_REG);// 0x2C
    SSD1680_SendData(0x78);// -3v

    SSD1680_SendCommand(SSD1680_GATE_VOLTAGE);// 0x03
    //SSD1680_HAL_SendData(0x17);// 20V
    SSD1680_SendData(0x03);// 10V

    SSD1680_SendCommand(SSD1680_SOURCE_VOLTAGE);// 0x04
    SSD1680_SendData(0x4B); // VSH1 = 17V
  //  EPD_HAL_SendData(0x28); // VSH1 = 10V
    SSD1680_SendData(0xA8); // VSH2 = 5V
    SSD1680_SendData(0x3A); // VSL = -17V
 //   EPD_HAL_SendData(0x1E); // VSL = -10

	SSD1680_SendCommand(SSD1680_WRITE_BORDER); //0x3C BorderWaveform
	SSD1680_SendData(0x60);	// 0x50 - black, 0x60 - white

	SSD1680_SendCommand(SSD1680_TEMP_CONTROL);// 0x18 Read built-in temperature sensor
	SSD1680_SendData(0x80);// Internal temperature sensor
}

void EPD_Border(bool black)
{
  	SSD1680_SendCommand(SSD1680_WRITE_BORDER); //0x3C BorderWaveform
	SSD1680_SendData(black?0x50:0x60);	// 0x50 - black, 0x60 - white
}

void EPD_Sleep()
{//To Exit Deep Sleep mode, User required to send HWRESET to the driver
	SSD1680_SendCommand(SSD1680_DEEP_SLEEP); //0x10 enter deep sleep
    SSD1680_SendData(0x02);//Deep Sleep Mode 2 (all off)
//	EDP_HAL_Delay_ms(100);
}

void EPD_Clean(bool fast)
{
    uint16_t size = EPD_IMAGE_SIZE;
    SSD1680_SendCommand(SSD1680_WRITE_BW_RAM);
    for(uint16_t j = 0; j < size; j++)
        SSD1680_SendData(0XFF);
    SSD1680_SendCommand(SSD1680_WRITE_RED_RAM);
    for(uint16_t j = 0; j < size; j++)
        SSD1680_SendData(0XFF);
    EPD_Update_Display((uint8_t*)0);
    uint16_t count = 0;
    if(fast)
    {
        count = 1;
        if(ssd1680_lut_delta[60] < 40)
            count ++;
        if(ssd1680_lut_delta[60] < 20)
            count ++;
    }
    for(uint16_t i = 0; i < count; i++)
    {
        SSD1680_SendCommand(SSD1680_WRITE_BW_RAM);
        for(uint16_t j = 0; j < size; j++)
            SSD1680_SendData(0x00);
        EPD_Update_Display(ssd1680_lut_delta);
        SSD1680_SendCommand(SSD1680_WRITE_BW_RAM);
        for(uint16_t j = 0; j < size; j++)
            SSD1680_SendData(0xFF);
        SSD1680_SendCommand(SSD1680_WRITE_RED_RAM);
        for(uint16_t j = 0; j < size; j++)
            SSD1680_SendData(0x00);
        EPD_Update_Display(ssd1680_lut_delta);
        SSD1680_SendCommand(SSD1680_WRITE_RED_RAM);
        for(uint16_t j = 0; j < size; j++)
            SSD1680_SendData(0XFF);
    }
}

void EPD_Draw(const uint8_t *bitImage, bool fast)
{
    SSD1680_SendCommand(SSD1680_WRITE_BW_RAM);
    SSD1680_SendData(bitImage, EPD_IMAGE_SIZE);
    EPD_Update_Display(fast?ssd1680_lut_delta:(uint8_t*)0);
    // save as previous image  for delta
    EPD_Load_RED(bitImage);
}

void EDP_DrawTicks(uint8_t ticks)
{
    if(ticks <  5)
        ticks = 5;
    if(ticks > 160)
        ticks = 160;
    ssd1680_lut_delta[60] = ticks;
}
