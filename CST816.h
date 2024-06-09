#ifndef _CST816_H
#define _CST816_H

#include <Wire.h>

#define __CST816_SLVADR__           0x15

#define __CST816_ADR_CHIPID__        0xA7
#define __CST816_ADR_PROJID__        0xA8
#define __CST816_ADR_FWVERSION__     0xA9
#define __CST816_ADR_MOTIONMASK__    0xEC
#define __CST816_ADR_IRQWIDTH__      0xED
#define __CST816_ADR_NORSCANPER__    0xEE
#define __CST816_ADR_MOTIONS1ANGLE__ 0xEF
#define __CST816_ADR_DEBOUCETIME__   0xFB
#define __CST816_ADR_LONGPRESSTIME__ 0xFC
#define __CST816_ADR_DISAUTOSLEEP__  0xFE

enum GESTURE
{
  None = 0x00,       //none
  SlideUp = 0x01,    //swipe up
  SlideDown = 0x02,  //swipe down
  SlideLeft = 0x03,  //swipe left
  SlideRight = 0x04, //swipe right
  SingleTap = 0x05,  //single click
  DoubleTap = 0x0B,  //double click
  LongPress = 0x0C   //long press
};

/**************************************************************************/
/*!
    @brief  CST816 I2C CTP controller driver
*/
/**************************************************************************/
class CST816
{
public:
  CST816(int8_t sda_pin = -1, int8_t scl_pin = -1, int8_t rst_pin = -1, int8_t int_pin = -1);
  CST816(TwoWire *w = NULL, int8_t rst_pin = -1, int8_t int_pin = -1);

  void begin(void);
  bool getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture);

private:
  TwoWire *wire;
  uint16_t _x, _y, _x_min, _x_max, _y_min, _y_max, _range_max;
  int8_t _sda, _scl, _rst, _int;
  uint8_t _touchCntr, _touchCntrPrev, _tmpGesture, iTmpDat[4], _deviceID;
  uint8_t _singleclick_max_, _click_cntr_, _click_timeout_cntr_, _noclick_cntr_;
  int8_t _click_timeout_;
  uint8_t i2c_read_byte(uint8_t addr);
  uint8_t i2c_read_nbyte(uint8_t addr, uint8_t *data, uint32_t length);
  void i2c_write(uint8_t addr, uint8_t data);
  uint8_t i2c_write_nbyte(uint8_t addr, const uint8_t *data, uint32_t length);
};
#endif