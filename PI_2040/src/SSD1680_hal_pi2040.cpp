#include <pico/time.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <boards/pico.h>
#include "hardware/spi.h"
#include "SSD1680_hal.h"


#define EPD_SPI_PORT  spi0
#define EPD_BUSY_PIN  0
#define EPD_DC_PIN    1
#define EPD_CLK_PIN   2
#define EPD_MOSI_PIN  3
#define EPD_CS_PIN    4
#define EPD_RST_PIN   5
//EPD_GND --> GND
//EPD_VCC --> 3V3


inline void PI2040_Digital_Write(uint16_t Pin, uint8_t Value)
{
	gpio_put(Pin, Value);
}

inline uint8_t PI2040_Digital_Read(uint16_t Pin)
{
	return gpio_get(Pin);
}

inline void PI2040_SPI_WriteByte(uint8_t Value)
{
    spi_write_blocking(EPD_SPI_PORT, &Value, 1);
}

inline void PI2040_SPI_Write_nByte(const uint8_t *pData, uint32_t len)
{
    spi_write_blocking(EPD_SPI_PORT, pData, len);
}

inline void PI2040_GPIO_Mode(uint16_t Pin, uint8_t Mode)
{
    gpio_init(Pin);
	gpio_set_dir(Pin, Mode);
}

inline void PI2040_Delay_ms(uint32_t xms)
{
	sleep_ms(xms);
}

void SSD1680_Init()
{
	PI2040_GPIO_Mode(EPD_RST_PIN, GPIO_OUT);
	PI2040_GPIO_Mode(EPD_DC_PIN, GPIO_OUT);
	PI2040_GPIO_Mode(EPD_CS_PIN, GPIO_OUT);
	PI2040_GPIO_Mode(EPD_BUSY_PIN, GPIO_IN);
	PI2040_Digital_Write(EPD_CS_PIN, GPIO_OUT);
	
    spi_init(EPD_SPI_PORT, 4000 * 1000);
    gpio_set_function(EPD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(EPD_MOSI_PIN, GPIO_FUNC_SPI);
}

void SSD1680_Reset()
{
    PI2040_Digital_Write(EPD_RST_PIN, 1);
    PI2040_Delay_ms(20);
    PI2040_Digital_Write(EPD_RST_PIN, 0);
    PI2040_Delay_ms(2);
    PI2040_Digital_Write(EPD_RST_PIN, 1);
    PI2040_Delay_ms(20);
}

static void EPD_HAL_ReadBusy()
{
	while(true)
	{	 //=1 BUSY
		if(PI2040_Digital_Read(EPD_BUSY_PIN) == 0)
			break;
		PI2040_Delay_ms(1);
	}
}

void SSD1680_SendCommand(uint8_t cmd)
{
    EPD_HAL_ReadBusy();
    PI2040_Digital_Write(EPD_DC_PIN, 0);
    PI2040_Digital_Write(EPD_CS_PIN, 0);
    PI2040_SPI_WriteByte(cmd);
    PI2040_Digital_Write(EPD_CS_PIN, 1);
}

void SSD1680_SendData(uint8_t data)
{
    PI2040_Digital_Write(EPD_DC_PIN, 1);
    PI2040_Digital_Write(EPD_CS_PIN, 0);
    PI2040_SPI_WriteByte(data);
    PI2040_Digital_Write(EPD_CS_PIN, 1);
}

void SSD1680_SendData(const uint8_t *pData, uint32_t len)
{
    PI2040_Digital_Write(EPD_DC_PIN, 1);
    PI2040_Digital_Write(EPD_CS_PIN, 0);
    PI2040_SPI_Write_nByte(pData, len);
    PI2040_Digital_Write(EPD_CS_PIN, 1);
}

