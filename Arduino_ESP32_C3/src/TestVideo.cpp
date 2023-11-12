#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>
#include "EPD.h"
#include "BitImage.h"

#include "./img/human01.h"
#include "./img/human02.h"
#include "./img/human03.h"
#include "./img/human04.h"
#include "./img/human05.h"
#include "./img/human06.h"
#include "./img/human07.h"
#include "./img/human08.h"
#include "./img/human09.h"
#include "./img/human10.h"
#include "./img/human11.h"
#include "./img/human12.h"
#include "./img/human13.h"
#include "./img/human14.h"
#include "./img/human15.h"
#include "./img/human16.h"


extern const unsigned char BitFont_DOS_V_re_JPN12_6x12[859];
extern const unsigned char BitFont_Trident_8x11[1049];
extern const unsigned char BitFont_DOS_V_re_ANK24_12x24[3424];
extern const unsigned char BitFont_Cordata_PPC_21_16x26[4944];


const size_t img_human_count = 16;
const uint8_t* img_human[16] = 
{
    bitImage_human1,  bitImage_human2,  bitImage_human3,  bitImage_human4,
    bitImage_human5,  bitImage_human6,  bitImage_human7,  bitImage_human8,
    bitImage_human9,  bitImage_human10, bitImage_human11, bitImage_human12,
    bitImage_human13, bitImage_human14, bitImage_human15, bitImage_human16
};




void TestVideo_Speed(BitImage& edp_img)
{
    unsigned long start_time, end_time;
    start_time = micros();
    for(size_t n = 0; n < 2; n++)
    {
        for(size_t i = 0; i < img_human_count; i++)
        {
            edp_img.DrawPicture(12, 30, img_human[i]);
            edp_img.DrawNumber(edp_img.m_width/2-12, 0, i, 2, BitFont_DOS_V_re_ANK24_12x24);
            EPD_Draw(edp_img.m_data, true);
        }
    }
    end_time = micros();
    uint32_t ms = (end_time - start_time) / 32 / 1000;

    edp_img.DrawString(5,      edp_img.m_height - 24, "FPS: ", BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawNumber(5+12*5, edp_img.m_height - 24, (1000/ms), 1, BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawString(5+12*6, edp_img.m_height - 24, ".", BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawNumber(5+12*7, edp_img.m_height - 24, (100000/ms)%100, 2, BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawString(5+12*8, edp_img.m_height - 24, " ", BitFont_DOS_V_re_ANK24_12x24);

    edp_img.DrawNumber(30,       edp_img.m_height - 30 - 12, ms, 4, BitFont_DOS_V_re_JPN12_6x12);
    edp_img.DrawString(30+6*4+2, edp_img.m_height - 30 - 12, "ms", BitFont_DOS_V_re_JPN12_6x12);
    EPD_Draw(edp_img.m_data, true);
}


void TestVideo(BitImage& edp_img)
{
    uint8_t ticks = 0;
   // uint8_t it = 0;
    while(true)
    {
        ticks += 5;
        if(ticks > 30)
            ticks+=5;
        if(ticks > 60)
        {
            ticks = 5;
          //  if(it++ > 2)
                break;
        }
        edp_img.Clear(true);
        EDP_DrawTicks(ticks);
        EPD_Clean(true);// dirty (no train)
        edp_img.DrawString(0,    edp_img.m_height - 24*3 - 8, "Ticks:", BitFont_DOS_V_re_ANK24_12x24);
        edp_img.DrawNumber(12*6, edp_img.m_height - 24*3 - 8, ticks, 2, BitFont_DOS_V_re_ANK24_12x24);
        edp_img.DrawString(5,    edp_img.m_height - 24, " ? Calc ?", BitFont_DOS_V_re_ANK24_12x24);
        TestVideo_Speed(edp_img);
        delay(5000);
    }
}
