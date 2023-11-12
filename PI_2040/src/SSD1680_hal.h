#pragma once
#include <stdint.h>

#define SSD1680_DRIVER_CONTROL     0x01
#define SSD1680_GATE_VOLTAGE       0x03
#define SSD1680_SOURCE_VOLTAGE     0x04
#define SSD1680_INIT_SETTING       0x08
#define SSD1680_INIT_WRITE_REG     0x09
#define SSD1680_INIT_READ_REG      0x0A
#define SSD1680_BOOSTER_SOFT_START 0x0C
#define SSD1680_DEEP_SLEEP         0x10
#define SSD1680_DATA_MODE          0x11
#define SSD1680_SW_RESET           0x12
#define SSD1680_HV_DETECT          0x14
#define SSD1680_VCI_DETECT         0x15
#define SSD1680_TEMP_CONTROL       0x18
#define SSD1680_TEMP_WRITE         0x1A
#define SSD1680_TEMP_READ          0x1B
#define SSD1680_EXTTEMP_WRITE      0x1C
#define SSD1680_MASTER_ACTIVATE    0x20
#define SSD1680_DISP_CTRL1         0x21
#define SSD1680_DISP_CTRL2         0x22
#define SSD1680_WRITE_BW_RAM       0x24
#define SSD1680_WRITE_RED_RAM      0x26
#define SSD1680_READ_RAM           0x27
#define SSD1680_VCOM_SENSE         0x28
#define SSD1680_VCOM_DURATION      0x29
#define SSD1680_WRITE_VCOM_OTP     0x2A
#define SSD1680_WRITE_VCOM_CTRL    0x2B
#define SSD1680_WRITE_VCOM_REG     0x2C
#define SSD1680_READ_OTP           0x2D
#define SSD1680_READ_USERID        0x2E
#define SSD1680_READ_STATUS        0x2F
#define SSD1680_WRITE_WS_OTP       0x30
#define SSD1680_LOAD_WS_OTP        0x31
#define SSD1680_WRITE_LUT          0x32
#define SSD1680_CRC_CALC           0x34
#define SSD1680_CRC_READ           0x35
#define SSD1680_PROG_OTP           0x36
#define SSD1680_WRITE_DISPLAY_OPT  0x37
#define SSD1680_WRITE_USERID       0x38
#define SSD1680_OTP_PROGMODE       0x39
#define SSD1680_WRITE_BORDER       0x3C
#define SSD1680_END_OPTION         0x3F
#define SSD1680_SET_RAM_X_POS      0x44
#define SSD1680_SET_RAM_Y_POS      0x45
#define SSD1680_AUTO_WRITE_RED     0x46
#define SSD1680_AUTO_WRITE_BW      0x47
#define SSD1680_SET_RAM_X_COUNT    0x4E
#define SSD1680_SET_RAM_Y_COUNT    0x4F
#define SSD1680_NOP                0xFF

#define SSD1680_LUT_SIZE 153

void SSD1680_Init();
void SSD1680_Reset();
void SSD1680_SendCommand(uint8_t cmd);
void SSD1680_SendData(uint8_t data);
void SSD1680_SendData(const uint8_t *pData, uint32_t len);
