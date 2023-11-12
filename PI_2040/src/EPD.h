#pragma once

#define EPD_WIDTH  122
#define EPD_HEIGHT 250

#define EPD_WIDTH_STEP ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1))
#define EPD_IMAGE_SIZE (EPD_WIDTH_STEP * EPD_HEIGHT)

void EPD_Init(bool halInit);
void EPD_Border(bool black);
void EPD_Sleep();// run EPD_Init(false); to wake up
void EPD_Clean(bool fast = false);
void EPD_Draw(const uint8_t *bitImage, bool fast = false);
void EDP_DrawTicks(uint8_t ticks);


//debug
void EPD_Load_BW(const uint8_t *blackImage);
void EPD_Load_RED(const uint8_t *redImage);
void EPD_Update_Display(const uint8_t* lut);
