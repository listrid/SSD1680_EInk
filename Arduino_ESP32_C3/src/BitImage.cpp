#include <string.h>
#include <stdlib.h>
#include "BitImage.h"

void BitImage::SetRotate(uint8_t rotate, bool flip)
{
     switch (rotate)
    {
        case BIT_IMAGE_ROTATE_180:
        case BIT_IMAGE_ROTATE_0:   m_width = m_real_width, m_height = m_real_height; break;
        case BIT_IMAGE_ROTATE_270:
        case BIT_IMAGE_ROTATE_90: m_width = m_real_height; m_height = m_real_width; break;
        default:
        {
            m_width = m_real_width, m_height = m_real_height;
            rotate = BIT_IMAGE_ROTATE_0;
        } 
    }
    m_rotate = rotate;
    m_flip   = flip;
}

BitImage::BitImage(uint8_t *data, uint16_t width, uint16_t height)
{
    m_data = NULL;
    Init(data, width, height);
}

BitImage::BitImage(uint16_t width, uint16_t height)
{
    m_data = NULL;
    Init(width, height);
}

void BitImage::Init(uint8_t *data, uint16_t width, uint16_t height)
{
    if(m_image_free && m_data)
        free(m_data);
    m_data       = data;
    m_real_width  = width;
    m_real_height = height;
    m_widthByte   = (width % 8 == 0) ? (width / 8) : (width / 8 + 1);
    m_image_free  = false;
    SetRotate(BIT_IMAGE_ROTATE_0, false);
}

void BitImage::Init(uint16_t width, uint16_t height)
{
    if(m_image_free && m_data)
        free(m_data);
    m_real_width  = width;
    m_real_height = height;
    m_widthByte   = (width % 8 == 0) ? (width / 8) : (width / 8 + 1);
    m_image_free  = true;
    m_data = (uint8_t*)malloc(m_widthByte*height);
    SetRotate(BIT_IMAGE_ROTATE_0, false);
}

BitImage::~BitImage()
{
    if(m_image_free && m_data)
        free(m_data);
}

void BitImage::Clear(bool value)
{
    memset(m_data, value ? 0xFF:0x00, m_real_height*m_widthByte);
}

void BitImage::InvertAllPixels()
{
    for(size_t i = 0; i < m_real_height*m_widthByte; i++)
        m_data[i] = ~m_data[i];
}

bool BitImage::GetPixel(uint16_t x, uint16_t y)
{
    int16_t X, Y;
    switch (m_rotate)
    {
        case BIT_IMAGE_ROTATE_0:   X = x, Y = y; break;
        case BIT_IMAGE_ROTATE_90:  X = y, Y = m_real_height - x - 1; break;
        case BIT_IMAGE_ROTATE_180: X = m_real_width - x - 1, Y = m_real_height - y - 1; break;
        case BIT_IMAGE_ROTATE_270: X = m_real_width - y - 1, Y = x; break;
    }
    if(uint16_t(X) >= m_real_width || uint16_t(Y) >= m_real_height)
        return false;
    if(m_flip)
        Y = m_real_height - Y - 1;
    uint32_t addr = (X >> 3) + Y * m_widthByte;
    return (m_data[addr] & (0x80 >> (X & 7))) != 0;
}

bool BitImage::SetPixel(uint16_t x, uint16_t y, bool value)
{
    int16_t X, Y;
    switch (m_rotate)
    {
        case BIT_IMAGE_ROTATE_0:   X = x, Y = y; break;
        case BIT_IMAGE_ROTATE_90:  X = y, Y = m_real_height - x - 1; break;
        case BIT_IMAGE_ROTATE_180: X = m_real_width - x - 1, Y = m_real_height - y - 1; break;
        case BIT_IMAGE_ROTATE_270: X = m_real_width - y - 1, Y = x; break;
    }
    if(uint16_t(X) >= m_real_width || uint16_t(Y) >= m_real_height)
        return false;
    if(m_flip)
        Y = m_real_height - Y - 1;
    uint32_t addr = (X >> 3) + Y * m_widthByte;
    if(value)
    {
        m_data[addr] |= (0x80 >> (X & 7));
    }else{
        m_data[addr] &= ~(0x80 >> (X & 7));
    }
    return true;
}

void BitImage::DrawLine(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, bool value)
{
    uint16_t Xpoint = x_start;
    uint16_t Ypoint = y_start;
    int32_t dx = (int)x_end - (int)x_start >= 0 ? x_end - x_start : x_start - x_end;
    int32_t dy = (int)y_end - (int)y_start <= 0 ? y_end - y_start : y_start - y_end;

    int32_t XAddway = x_start < x_end ? 1 : -1;
    int32_t YAddway = y_start < y_end ? 1 : -1;
    int32_t Esp = dx + dy;

    for(;;)
    {
        SetPixel(Xpoint, Ypoint, value);
        if (2 * Esp >= dy)
        {
            if (Xpoint == x_end)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx)
        {
            if (Ypoint == y_end)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

void BitImage::DrawTriangle(size_t Ax, size_t Ay, size_t Bx, size_t By, size_t Cx, size_t Cy, bool fill, bool value)
{
    if(!fill)
    {
        DrawLine(Ax, Ay, Bx, By, value);
        DrawLine(Bx, By, Cx, Cy, value);
        DrawLine(Cx, Cy, Ax, Ay, value);
        return;
    }
    size_t tmp, x1, x2;
    if(Ay > By)
    {
        tmp = Ay; Ay = By; By = tmp;
        tmp = Ax; Ax = Bx; Bx = tmp;
    }
    if(Ay > Cy)
    {
        tmp = Ay; Ay=Cy; Cy=tmp;
        tmp = Ax; Ax=Cx; Cx=tmp;
    }
    if(By > Cy)
    {
        tmp = Cy; Cy = By; By = tmp;
        tmp = Cx; Cx = Bx; Bx = tmp;
    }
    for(size_t sy = Ay; sy <= Cy; sy++)
    {
        if(Cy == Ay)
            x1 = Ax;
        else
            x1 = Ax + int(sy - Ay) * int(Cx - Ax) / int(Cy - Ay);
        if(sy < By)
            x2 = Ax + int(sy - Ay) * int(Bx - Ax) / int(By - Ay);
        else{
            if(Cy == By)
                x2 = Bx;
            else
                x2 = Bx + int(sy - By) * int(Cx - Bx) / int(Cy - By);
        }
        if(x1 > x2)
        {
            tmp = x1; x1 = x2; x2 = tmp;
        }
        DrawLine(x1, sy, x2, sy, value);
    }
}

void BitImage::DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool fill, bool value)
{
    uint16_t x_end = x+width;
    uint16_t y_end = y+height; 
    if(fill)
    {
        for(uint16_t i = y; i < y_end; i++)
        {
            DrawLine(x, i, x_end, i, value);
        }
    }else{
        DrawLine(x,         y, x_end,     y, value);
        DrawLine(x,     y_end, x_end, y_end, value);
        DrawLine(x,         y,     x, y_end, value);
        DrawLine(x_end,     y, x_end, y_end, value);
    }
}

void BitImage::DrawCircle(uint16_t x, uint16_t y, uint16_t radius, bool fill, bool value)
{
    int32_t w = int32_t(m_real_width);
    int32_t h = int32_t(m_real_height);
    int32_t x1 = 0;
    int32_t y1 = int32_t(radius);
    int32_t delta = int32_t(2 * (1 - radius));
    do
    {
        if(fill)
        {
            int32_t xS = int32_t(x)-x1;
            int32_t xE = int32_t(x)+x1;
            if(xS < 0)
                xS = 0;
            if(xE > w)
                xE = w;
            if(int32_t(y + y1) < h && int32_t(y + y1) >= 0)
                DrawLine(xS, y+y1, xE, y+y1, value);
            if(int32_t(y - y1) < h && int32_t(y - y1) >= 0)
                DrawLine(xS, y-y1, xE, y-y1, value);
        }else{
            if(int32_t(y + y1) < h && int32_t(y + y1) >= 0)
            {
                if(int32_t(x + x1) < w && (x + x1) >= 0)
                    SetPixel(x + x1, y + y1, value);
                if(int32_t(x - x1) < w && (x - x1) >= 0)
                    SetPixel(x - x1, y + y1, value);
            }
            if(int32_t(y - y1) < h && int32_t(y - y1) >= 0)
            {
                if(int32_t(x + x1) < w && int32_t(x + x1) >= 0)
                    SetPixel(x + x1, y - y1, value);
                if(int32_t(x - x1) < w && int32_t(x - x1) >= 0)
                    SetPixel(x - x1, y - y1, value);
            }
        }
        if(y1 < 0)
            break;
        bool next = true;
        if(delta < 0)
        {
            int32_t sigma = 2 * (delta + y1) - 1;
            if(sigma <= 0)
            {
                x1++;
                delta += 2 * x1 + 1;
                next = false;
            }
        }else if(delta > 0)
        {
            int32_t sigma = 2 * (delta - x1) - 1;
            if(sigma > 0)
            {
                y1--;
                delta += 1 - 2 * y1;
                next = false;
            }
        }
        if(next)
        {
            x1++;
            y1--;
            delta += 2 * (x1 - y1 - 1);
        }
    }while(true);
}

void BitImage::DrawChar(uint16_t x, uint16_t y, uint8_t ch, const uint8_t* font_data, bool invert)
{
    size_t ch_w = font_data[0];
    size_t ch_h = font_data[1];
    size_t ch_s = font_data[2];

    if(font_data[3]&1)//BIT_FONT_FLAG_95_CHARS
    {
        ch -= ' ';
        if(ch >= 95)
            ch = '?' - ' ';
    }
    const uint8_t* pos = &font_data[ch_s*ch + 4];
    size_t mask_bit = 0;
    uint8_t pix8;
    for(size_t h = 0; h < ch_h; h++)
    {
        for(uint16_t w = 0; w < ch_w; w++)
        {
            mask_bit >>= 1;
            if(mask_bit == 0)
            {
                mask_bit = 0x80;
                pix8 = *pos++;
            }
            if (pix8 & mask_bit)
                SetPixel(x + w, y, invert);
            else
                SetPixel(x + w, y, !invert);
        }
        y++;
    }
}

void BitImage::DrawString(uint16_t x, uint16_t y, const char *str, const uint8_t* font_data, bool invert)
{
    while(*str != '\0')
    {
        DrawChar(x, y, *str++, font_data, invert);
        x += font_data[0];
    }
}

void BitImage::DrawNumber(uint16_t x, uint16_t y, uint32_t num, uint8_t len, const uint8_t* font_data, bool invert)
{
    for(uint8_t t = 1; t <= len; t++)
    {
        DrawChar(x + font_data[0] * (len - t), y,  '0' + num%10, font_data, invert);
        num /= 10;
    }
}

void BitImage::DrawPicture(uint16_t x, uint16_t y, const uint8_t* bitImage, bool invert, uint16_t maxWidth, uint16_t maxHeight)
{
    uint16_t imgWidth  = ((uint16_t*)bitImage)[0];
    uint16_t imgHeight = ((uint16_t*)bitImage)[1];
    uint16_t imgStep   = (imgWidth+7)>>3;
    bitImage += 4;

    if(maxWidth > imgWidth)
        maxWidth = imgWidth;
    if(maxHeight > imgHeight)
        maxHeight = imgHeight;
    uint16_t startX = x;
    maxWidth +=x ;
    for(maxHeight += y; y < maxHeight; y++)
    {
        size_t j = 0;
        size_t mask_bit = 0;
        uint8_t pix8;
        for(x = startX; x < maxWidth; x++)
        {
            if(!mask_bit)
            {
                pix8 = bitImage[j++];
                mask_bit = 0x80;
            }
            SetPixel(x, y, (pix8 & mask_bit)?(!invert):(invert));
            mask_bit >>= 1;
        }
        bitImage += imgStep;
    }
}

void BitImage::DrawPicture(uint16_t x, uint16_t y, const BitImage* bitImage, bool mix, bool mixValue)
{
    uint16_t imgWidth  = bitImage->m_width;
    uint16_t imgHeight = bitImage->m_height;
    uint16_t imgStep   = (imgWidth+7)>>3;
    const uint8_t* data = bitImage->m_data;

    uint16_t startX = x;
    imgWidth +=x ;
    for(imgHeight += y; y < imgHeight; y++)
    {
        size_t j = 0;
        size_t mask_bit = 0;
        uint8_t pix8;
        for(x = startX; x < imgWidth; x++)
        {
            if(!mask_bit)
            {
                pix8 = data[j++];
                mask_bit = 0x80;
            }
            if(mix)
            {
                if(((pix8 & mask_bit) != 0) == mixValue)
                    SetPixel(x, y, mixValue);
            }else{
                SetPixel(x, y, (pix8 & mask_bit) != 0);
            }
            mask_bit >>= 1;
        }
        data += imgStep;
    }
}
