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
    delay(1000);

    edp_img.SetRotate(BIT_IMAGE_ROTATE_90);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2-12, edp_img.m_height/2 - 6*3, "90#", BitFont_DOS_V_re_ANK24_12x24);
    edp_img.DrawString(5, edp_img.m_height/2+16, "https://github.com/listrid/SSD1680_EInk", BitFont_DOS_V_re_JPN12_6x12);
    EPD_Draw(edp_img.m_data, true);
    delay(1000);
    edp_img.SetRotate(BIT_IMAGE_ROTATE_180);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2-6*4, edp_img.m_height/2-12, "180`", BitFont_DOS_V_re_ANK24_12x24);
    EPD_Draw(edp_img.m_data, true);
    delay(1000);
    edp_img.SetRotate(BIT_IMAGE_ROTATE_270);
    DrawNumAngle(edp_img);
    edp_img.DrawString(edp_img.m_width/2 - 12, edp_img.m_height/2 - 6*4, "270~", BitFont_DOS_V_re_ANK24_12x24);
    EPD_Draw(edp_img.m_data, true);
    delay(1000);
    edp_img.SetRotate(BIT_IMAGE_ROTATE_0);
}

void setup() {
    EPD_Init(true);
    EPD_Clean(); // high quality
}

void loop() {
    BitImage edp_img(image_bw, EPD_WIDTH, EPD_HEIGHT);
    for(size_t i = 1; i < 2; i++)
        TestRotate(edp_img);
    TestGray(edp_img);  
    TestVideo(edp_img);
}

