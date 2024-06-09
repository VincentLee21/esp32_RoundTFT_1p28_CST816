#ifndef __DEV_CONFIG_H__
#define __DEV_CONFIG_H__

#include "stdio.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "DEV_Debug.h"

#ifdef CONFIG_IDF_TARGET_ESP32C3
#define __ESP32C3__
#endif

#define __GPIO_PWM__


#ifdef __ESP32C3__
// TFT backlight pins
#define LCD_BL_PIN          3

// i2c IO pins
#define DEV_SDA0_PIN        4
#define DEV_SCL0_PIN        5

// touchscreen IO pins
#define TP_I2C_SCL          DEV_SCL0_PIN
#define TP_I2C_SDA          DEV_SDA0_PIN
#define TP_IO_RST           1
#define TP_IO_INT           0
#define TP_RST_ACTIVE       0
#define TP_RST_INACTIVE     1
#define TP_INT_ACTIVE       1

#else // ESP32S3
// TFT backlight pins
#define LCD_BL_PIN          2

// i2c IO pins
#define DEV_SDA0_PIN        6
#define DEV_SCL0_PIN        7
#define DEV_SDA1_PIN        15
#define DEV_SCL1_PIN        16

// IO for LED pins
#define DEV_LED_R_PIN       18
#define DEV_LED_G_PIN       17
#define DEV_LED_B_PIN       21

// touchscreen IO pins
#define TP_I2C_SCL          DEV_SCL0_PIN
#define TP_I2C_SDA          DEV_SDA0_PIN
#define TP_IO_RST           13
#define TP_IO_INT           5
#define TP_RST_ACTIVE       0
#define TP_RST_INACTIVE     1
#define TP_INT_ACTIVE       1

// ADC IO pins
#define BAT_ADC_PIN         1
#endif

//--------------------
// Function definition
//--------------------

void DEV_Digital_Write(uint16_t Pin, uint8_t Value);
uint8_t DEV_Digital_Read(uint16_t Pin);
#ifndef __ESP32C3__
float DEV_ADC_Read(void);
#endif

void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode);
void DEV_GPIO_IN_PULLUP(uint16_t Pin);

#ifdef __GPIO_PWM__
void DEV_GPIO_PWM( int16_t pin, int16_t pwm_channel, int16_t frequency, int16_t resolution);
void DEV_GPIO_PWM_dutycycle( int16_t pwm_channel, int16_t dutycycle);
#else
void DEV_GPIO_PWM(int16_t pin, uint8_t Value);
#endif

void DEV_I2C0_Write_Byte(uint8_t addr, uint8_t reg_addr, uint8_t Value);
void DEV_I2C0_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len);
void DEV_I2C0_Write_Register(uint8_t addr,uint8_t reg_addr, uint16_t value);

uint8_t DEV_I2C0_Read_Byte(uint8_t addr, uint8_t reg);
void DEV_I2C0_Read_nByte(uint8_t addr, uint8_t reg_addr, uint8_t *pData, uint32_t Len);
void DEV_I2C0_Read_Register(uint8_t addr,uint8_t reg_addr, uint16_t *value);

#ifndef __ESP32C3__
void DEV_I2C1_Write_Byte(uint8_t addr, uint8_t reg_addr, uint8_t Value);
void DEV_I2C1_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len);
void DEV_I2C1_Write_Register(uint8_t addr,uint8_t reg_addr, uint16_t value);

uint8_t DEV_I2C1_Read_Byte(uint8_t addr, uint8_t reg);
void DEV_I2C1_Read_nByte(uint8_t addr, uint8_t reg_addr, uint8_t *pData, uint32_t Len);
void DEV_I2C1_Read_Register(uint8_t addr,uint8_t reg_addr, uint16_t *value);
#endif

uint8_t DEV_Module_Init(void);


class LEDPWM {
  private:
  int         _pin;
  uint32_t    _frequency;
  uint32_t    _dutycycle;
  char        _name[16];
  uint8_t     _channel;
  uint8_t     _resolution;

  public:
  LEDPWM(void);
  ~LEDPWM(void);

  void Initialize(char *name, uint32_t pin, uint8_t channel,
                  uint32_t frequency, uint8_t resolution);
  void SetDutyCycle(uint32_t dutycycle);
};

#endif
