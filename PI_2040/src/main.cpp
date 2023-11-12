#include <pico/time.h>

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <pico/stdio_usb.h>
#include <boards/pico.h>
#include "EPD.h"
#include "BitImage.h"


extern const unsigned char BitFont_DOS_V_re_JPN12_6x12[859];
extern const unsigned char BitFont_DOS_V_re_ANK24_12x24[3424];

uint8_t image_bw[EPD_IMAGE_SIZE];

void TestGray(BitImage& edp_img);
void TestVideo(BitImage& edp_img);

void DrawNumAngle(BitImage& edp_img)
{
    edp_img.Clear(true);
    edp_img.DrawNumber(0, 0, 1, 1, BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawNumber(edp_img.m_width - 18, 0, 2, 1, BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawNumber(edp_img.m_width - 18, edp_img.m_height - 25, 3, 1, BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawNumber(0, edp_img.m_height - 25, 4, 1, BitFont_DOS_V_re_ANK24_12x24);
}

void TestRotate(BitImage& edp_img)
{
    edp_img.SetRotate(BIT_IMAGE_ROTATE_0);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2 - 6*2, edp_img.m_height/2-12, "0%", BitFont_DOS_V_re_ANK24_12x24);
    EPD_Draw(edp_img.m_data, true);
    sleep_ms(1000);

    edp_img.SetRotate(BIT_IMAGE_ROTATE_90);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2-12, edp_img.m_height/2 - 6*3, "90#", BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawString(5, edp_img.m_height/2+16, "https://github.com/listrid/SSD1680_EInk", BitFont_DOS_V_re_JPN12_6x12);
    EPD_Draw(edp_img.m_data, true);
    sleep_ms(1000);

    edp_img.SetRotate(BIT_IMAGE_ROTATE_180);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2-6*4, edp_img.m_height/2-12, "180`", BitFont_DOS_V_re_ANK24_12x24);
    EPD_Draw(edp_img.m_data, true);
    sleep_ms(1000);

    edp_img.SetRotate(BIT_IMAGE_ROTATE_270);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2 - 12, edp_img.m_height/2 - 6*4, "270~", BitFont_DOS_V_re_ANK24_12x24);
    EPD_Draw(edp_img.m_data, true);
    sleep_ms(1000);
    edp_img.SetRotate(BIT_IMAGE_ROTATE_0);
}


int main()
{
    stdio_init_all();
	stdio_set_driver_enabled(&stdio_usb, true);

    EPD_Init(true);
    EPD_Clean(); // high quality
    BitImage edp_img(image_bw, EPD_WIDTH, EPD_HEIGHT);
    while(true)
    {
        for(size_t i = 1; i < 2; i++)
            TestRotate(edp_img);
        TestGray(edp_img);  
        TestVideo(edp_img);
    }



    BitImage img(image_bw, EPD_WIDTH, EPD_HEIGHT);
    img.SetRotate(BIT_IMAGE_ROTATE_0);


	while(true)
		sleep_ms(1000);

img.DrawLine(0, 30, 0, 50, false);
img.DrawLine(0,50, 90, 50, false);
img.DrawLine(90-10,50+5, 90, 50, false);
img.DrawLine(90-10,50-5, 90, 50, false);

EPD_Draw(image_bw, true);
EPD_Border(false);
img.DrawCircle(0,0, 9, false, false);
for(size_t i = 1; i < 100; i++)
{
    img.DrawCircle((i-1)*2,(i-1)*2, 9, true, false);
    img.DrawCircle(i*2,(i*2), 9, false, false);

img.DrawNumber(10, 100, i, 2, BitFont_DOS_V_re_ANK24_12x24);

    EPD_Draw(image_bw, true);

//    sleep_ms(1000);
}
EPD_Sleep();
	while(true)
		sleep_ms(1000);



    return 0;
}