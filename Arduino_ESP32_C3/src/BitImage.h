#pragma once

#include <stdint.h>


#define BIT_IMAGE_ROTATE_0   0
#define BIT_IMAGE_ROTATE_90  1
#define BIT_IMAGE_ROTATE_180 2
#define BIT_IMAGE_ROTATE_270 3

// pix8 { 0pix->7bit, .., 7pix->0bit } 

class BitImage
{
    bool     m_image_free;
    uint16_t m_real_width;
    uint16_t m_real_height;
    uint8_t  m_rotate;
    bool     m_flip;
    uint16_t m_widthByte;
public:
    uint8_t* m_data;
    uint16_t m_width;
    uint16_t m_height;

	BitImage(uint8_t *data, uint16_t width, uint16_t height);
    BitImage(uint16_t width, uint16_t height);
	~BitImage();
    void Init(uint8_t *data, uint16_t width, uint16_t height);
    void Init(uint16_t width, uint16_t height);
    void SetRotate(uint8_t rotate, bool flip = false);

    void Clear(bool value);
    void InvertAllPixels();

    bool GetPixel(uint16_t x, uint16_t y);
    bool SetPixel(uint16_t x, uint16_t y, bool value);
    void DrawLine(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, bool value);
    void DrawTriangle(size_t Ax, size_t Ay, size_t Bx, size_t By, size_t Cx, size_t Cy, bool fill, bool value);
    void DrawRectangle(uint16_t x_start, uint16_t Y_start, uint16_t width, uint16_t height, bool fill, bool value);    
    void DrawCircle(uint16_t x, uint16_t y, uint16_t radius, bool fill, bool value);

    void DrawChar(uint16_t x, uint16_t y, uint8_t chr, const uint8_t* font_data, bool invert = false);
    void DrawString(uint16_t x, uint16_t y, const char *str, const uint8_t* font_data, bool invert = false);
    void DrawNumber(uint16_t x, uint16_t y, uint32_t num, uint8_t len, const uint8_t* font_data, bool invert = false);

    void DrawPicture(uint16_t x, uint16_t y, const uint8_t* bitImage, bool invert = true, uint16_t maxWidth = ~0, uint16_t maxHeight = ~0);
    void DrawPicture(uint16_t x, uint16_t y, const BitImage* bitImage, bool mix = false, bool mixValue = true);
};

