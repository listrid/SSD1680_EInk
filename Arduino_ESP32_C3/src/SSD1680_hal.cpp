#include <Arduino.h>
#include <SPI.h>
#include "SSD1680_hal.h"


#define EPD_BUSY_PIN  1
#define EPD_RESET_PIN 2
#define EPD_DC_PIN    3
#define EPD_SCL_PIN   4  // SCK
#define EPD_MISO_PIN  5  // not use
#define EPD_SDA_PIN   6  // MOSI
#define EPD_CS_PIN    7  // CS/SS


//EPD_GND --> GND
//EPD_VCC --> 3V3


inline void SPI_WriteByte(uint8_t Value)
{
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(EPD_CS_PIN, LOW);
  SPI.transfer(Value);
  digitalWrite(EPD_CS_PIN, HIGH);
  SPI.endTransaction();
}

inline void SPI_Write_nByte(const uint8_t *pData, uint32_t len)
{
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(EPD_CS_PIN, LOW); 
  for (uint16_t i = 0; i < len; i++)
  {
    SPI.transfer(*pData++);
  }
  digitalWrite(EPD_CS_PIN, HIGH); 
  SPI.endTransaction();
}


void SSD1680_Init()
{
    pinMode(EPD_BUSY_PIN,  INPUT);
    pinMode(EPD_RESET_PIN, OUTPUT);
    pinMode(EPD_DC_PIN,    OUTPUT);
  	pinMode(EPD_CS_PIN,    OUTPUT);
    SPI.end();
    SPI.begin(EPD_SCL_PIN, EPD_MISO_PIN, EPD_SDA_PIN, EPD_CS_PIN);
}

void SSD1680_Reset()
{
    digitalWrite(EPD_RESET_PIN, HIGH);
    delay(20);
    digitalWrite(EPD_RESET_PIN, LOW);
    delay(2);
    digitalWrite(EPD_RESET_PIN, HIGH);
    delay(20);
}

static void EPD_HAL_ReadBusy()
{
	while(true)
	{	 //=1 BUSY
		if(digitalRead(EPD_BUSY_PIN) == 0)
			break;
		delay(1);
	}
}

void SSD1680_SendCommand(uint8_t cmd)
{
    EPD_HAL_ReadBusy();
    digitalWrite(EPD_DC_PIN, LOW);
    SPI_WriteByte(cmd);
}

void SSD1680_SendData(uint8_t data)
{
    digitalWrite(EPD_DC_PIN, HIGH);
    SPI_WriteByte(data);
}

void SSD1680_SendData(const uint8_t *pData, uint32_t len)
{
    digitalWrite(EPD_DC_PIN, HIGH);
    SPI_Write_nByte(pData, len);
}

