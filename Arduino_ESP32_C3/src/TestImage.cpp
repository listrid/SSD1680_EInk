#include <string.h>
#include <Arduino.h>
#include "SSD1680_hal.h"
#include "EPD.h"
#include "BitImage.h"

#include "./img/Lion.h"
#include "./img/Asimov.h"
#include "./img/Einstein.h"

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

static uint8_t ssd1680_lut_test[SSD1680_LUT_SIZE] =
{
/*    Phase 0     Phase 1     Phase 2     Phase 3     Phase 4     Phase 5     Phase 6     Phase 7     Phase 8     Phase 9     Phase 10    Phase 11  */
/*VS  A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D     A B C D   */
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut0 (wb == 0 && red == 0) */
    0b10000000, 0b01000000, 0b10000000, 0b01000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut1 (wb == 1 && red == 0) */
    0b01000000, 0b10000000, 0b01000000, 0b10000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut2 (wb == 0 && red == 1) */
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut3 (wb == 1 && red == 1) */
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, /* lut4 ( ??? ) */
/*   TP[] - duration, SR[] - Repeats   */
/* TP[A] TP[B] SR[AB] TP[C] TP[D]  SR[CD] |	Repeats phase */
     1,   0,    0,     0,     0,    0,       0,     /* Phase 0  */
     1,   0,    0,     0,     0,    0,       0,     /* Phase 1  */
     1,   0,    0,     0,     0,    0,       0,     /* Phase 2  */
     1,   0,    0,     0,     0,    0,       0,     /* Phase 3  */
     1,   0,    0,     0,     0,    0,       0,     /* Phase 4  */
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


static uint8_t ssd1680_color[] = {   2,   3,    4,   6,   8,  11, 16, 25, 80};
static uint8_t ssd1680_tresh[] = { 200, 180,  160, 140, 120, 100, 80, 60,  0};

void TestGray_SetColor(uint8_t color)
{
    if(color >= sizeof(ssd1680_color))
        color = 0;
    ssd1680_lut_test[60] = ssd1680_color[color];
    if(color != 0)
    {
        ssd1680_lut_test[60+7*1] = ssd1680_color[color];
        ssd1680_lut_test[60+7*2] = ssd1680_color[color];
    }else{
        ssd1680_lut_test[60+7*1] = 0;
        ssd1680_lut_test[60+7*2] = 0;
    }
    ssd1680_lut_test[60+7*6] = ssd1680_color[color];
}


static void TestGray_DrawColor(BitImage& edp_img, uint8_t color, const uint8_t* img_data, size_t img_width, size_t img_height)
{
    uint8_t treshold = ssd1680_tresh[color];
    if(color == 0)
        edp_img.Clear(true);
    for(size_t h = 0; h < img_height; h++)
    {
        for(size_t w = 0; w < img_width; w++)
        {
            if(img_data[h*img_width + w] >= treshold)
                edp_img.SetPixel(h, img_width-1-w, false);
        }
    }
    if(color == 0)
    {
        EPD_Clean();
        EPD_Load_RED(edp_img.m_data);
    }
    EPD_Load_BW(edp_img.m_data);
    TestGray_SetColor(color);
    EPD_Update_Display(ssd1680_lut_test);
}

void TestGray(BitImage& edp_img)
{
    EPD_Clean();
    edp_img.Clear(true);
    EPD_Load_BW(edp_img.m_data);
    TestGray_SetColor(0);
    EPD_Update_Display(ssd1680_lut_test);
    for(size_t i = 0; i < 10 ;i++)
    {
        edp_img.DrawRectangle(0, 25*(i+1), edp_img.m_width, 25, true, false);
        EPD_Load_BW(edp_img.m_data);
        TestGray_SetColor(i);
        EPD_Update_Display(ssd1680_lut_test);
    }

    delay(3000);
    for(size_t i = 0; i < sizeof(ssd1680_tresh) ;i++)
        TestGray_DrawColor(edp_img, i, gLion_data, gLion_width, gLion_height);
    delay(10000);
    for(size_t i = 0; i < sizeof(ssd1680_tresh) ;i++)
        TestGray_DrawColor(edp_img, i, gEinstein_data, gEinstein_width, gEinstein_height);
    delay(10000);
    for(size_t i = 0; i < sizeof(ssd1680_tresh) ;i++)
        TestGray_DrawColor(edp_img, i, gAsimov_data, gAsimov_width, gAsimov_height);
    delay(10000);
}
