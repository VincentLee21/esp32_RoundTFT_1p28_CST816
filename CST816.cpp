#include <Arduino.h>
#include "CST816.h"

CST816::CST816(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin)
{
  wire = NULL;
  _sda = sda_pin;
  _scl = scl_pin;
  _rst = rst_pin;
  _int = int_pin;
}

CST816::CST816(TwoWire *w, int8_t rst_pin, int8_t int_pin) {
  wire = w;
  _sda = -1;
  _scl = -1;
  _rst = rst_pin;
  _int = int_pin;
}


void CST816::begin(void)
{
  // Initialize I2C
  if (_sda != -1 && _scl != -1)
  {
    if( wire == NULL) {
      Wire.begin(_sda, _scl, 400000);
      wire = &Wire;
    }
  }

  // Int Pin
  if (_int != -1)
  {
      pinMode(_int, OUTPUT);
      digitalWrite(_int, HIGH);
      delay(1);
      digitalWrite(_int, LOW);
      delay(1);
  }

  // Reset Pin
  if (_rst != -1)
  {
      pinMode(_rst, OUTPUT);
      digitalWrite(_rst, LOW);
      delay(10);
      digitalWrite(_rst, HIGH);
      delay(300);
  }

  _singleclick_max_ = 5; // 5 measurements
  _click_cntr_ = 0;
  _click_timeout_ = 40;
  _x_min = 1000;
  _x_max = 0;
  _y_min = 1000;
  _y_max = 0;
  _range_max = 10;

  // Initialize Touch
  i2c_read_nbyte( __CST816_ADR_CHIPID__, iTmpDat, 3);
  _deviceID = iTmpDat[0];
  Serial.printf("DevID # 0x%02X, ProjID 0x%02X, FwVer 0x%02X\n",
    _deviceID, iTmpDat[1], iTmpDat[2]);

  switch( _deviceID) {
    case 0xB4: Serial.println("CST816S detected"); break;
    case 0xB5: Serial.println("CST816T detected"); break;
    case 0xB6: Serial.println("CST816D detected"); break;
    default:   Serial.println("Unknown device detected"); break;
  }

  i2c_write( __CST816_ADR_DISAUTOSLEEP__, 0XFF); // disable auto sleep
  i2c_write( __CST816_ADR_NORSCANPER__, 0x01);
  // i2c_write( __CST816_ADR_MOTIONS1ANGLE__, 0x06);
  i2c_write( __CST816_ADR_DEBOUCETIME__, 0x32);
  i2c_write( __CST816_ADR_LONGPRESSTIME__, 0x1C);
}

bool CST816::getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture)
{
  int x_range, y_range, x_in_range, y_in_range;
  uint8_t data[4], dat1;
  bool FingerIndex = false;

  dat1 = i2c_read_byte(0x01);
  FingerIndex = (bool)i2c_read_byte(0x02);
  i2c_read_nbyte(0x03,data,4);
  _x = ((data[0] & 0x0f) << 8) | data[1];
  _y = ((data[2] & 0x0f) << 8) | data[3];

  if( FingerIndex && _touchCntr < 0x0F)
    _touchCntr += 1;
  else if((FingerIndex == false) && ( _touchCntr > 0))
    _touchCntr -= 1;

  if( FingerIndex) {
    _x_min = (_x < _x_min) ? _x : _x_min;
    _x_max = (_x > _x_max) ? _x : _x_max;
    _y_min = (_y < _y_min) ? _y : _y_min;
    _y_max = (_y > _y_max) ? _y : _y_max;
  } else {
  }
  _click_timeout_cntr_ = ((_click_timeout_cntr_ < _click_timeout_) && (_click_timeout_cntr_ > 0)) ? _click_timeout_cntr_ + 1 : _click_timeout_cntr_;

  x_range = (_x_max-_x_min);
  y_range = (_y_max-_y_min);
  x_in_range = x_range < _range_max;
  y_in_range = y_range < _range_max;

  if( _touchCntr == 0 && _touchCntrPrev == 1){
    _x_min = 1000;
    _x_max = 0;
    _y_min = 1000;
    _y_max = 0;
  }
  else if(( _touchCntrPrev > _touchCntr) && (_touchCntr <= _singleclick_max_)) { //relased finger
    if(( dat1==0) && x_in_range && y_in_range) {
      _click_cntr_ = 1;

      if( _click_timeout_cntr_ > (_click_timeout_>>2)) // next click within timeout
        _click_cntr_ = 2;

      if( _click_timeout_cntr_ == 0) // kick start timeout cntr
        _click_timeout_cntr_ = 1;
    }
  } else {
    // _tmpGesture = i2c_read_byte(0x01);
    _click_cntr_ = 0;
  }

  if( dat1 == 0) { // no gesture from cst816
    if( _click_cntr_==1)
      _tmpGesture = SingleTap;
    else if( _click_cntr_==2)
      _tmpGesture = DoubleTap;
  } else {
    _tmpGesture = dat1;
  }

  if( _touchCntr > 0)
    *gesture = _tmpGesture;

  // if( _touchCntr > 0) {
  //   // Serial.printf( "_touchCntr %d, _touchCntrPrev %d, _tmpGesture %d\n",
  //   //       _touchCntr, _touchCntrPrev, _tmpGesture);
  //   Serial.printf( "_touchCntr  %d, _tmpGesture %Xh, gesture %Xh, dat1 %d, x_in_range %d, y_in_range %d, _click_cntr_ %d, _click_timeout_cntr_ %d\n",
  //         _touchCntr, _tmpGesture, *gesture, dat1, x_in_range, y_in_range, _click_cntr_, _click_timeout_cntr_);
  //   // Serial.printf( "_touchCntr  %d, gesture %d, _click_cntr_ %d, _x %d, _y %d, _x_min %d, _x_max %d, _y_min %d, _y_max %d\n",
  //   //       _touchCntr, *gesture, _click_cntr_, _x, _y, _x_min, _x_max, _y_min, _y_max);
  // }

  if( _click_timeout_cntr_ == _click_timeout_) {
    _click_timeout_cntr_ = 0;
    _click_cntr_ = 0;
    _x_min = 1000;
    _x_max = 0;
    _y_min = 1000;
    _y_max = 0;
  }
  *x = _x;
  *y = _y;

  _touchCntrPrev = _touchCntr;
  return FingerIndex;
}

uint8_t CST816::i2c_read_byte(uint8_t addr)
{
  uint8_t rdData;
  uint8_t rdDataCount;

  if( wire == NULL)
    return 0;

  do
  {
    wire->beginTransmission( __CST816_SLVADR__);
    wire->write(addr);
    wire->endTransmission( false); // Restart
    rdDataCount = wire->requestFrom( __CST816_SLVADR__, 1);
  } while( rdDataCount == 0);

  while( wire->available())
    rdData = wire->read();

  return rdData;
}

uint8_t CST816::i2c_read_nbyte( uint8_t addr, uint8_t *data, uint32_t length)
{
  if( wire == NULL)
    return 0;

  wire->beginTransmission( __CST816_SLVADR__);
  wire->write(addr);

  if( wire->endTransmission( true))
    return -1;

  wire->requestFrom( __CST816_SLVADR__, length);

  for( int i = 0; i < length; i++)
    *data++ = wire->read();

  return 0;
}

void CST816::i2c_write(uint8_t addr, uint8_t data)
{
  if( wire == NULL)
    return;

  wire->beginTransmission( __CST816_SLVADR__);
  wire->write( addr);
  wire->write( data);
  wire->endTransmission();
}

uint8_t CST816::i2c_write_nbyte( uint8_t addr, const uint8_t *data, uint32_t length)
{
  if( wire == NULL)
    return 0;

  wire->beginTransmission( __CST816_SLVADR__);
  wire->write( addr);
  for( int i = 0; i < length; i++) {
    wire->write( *data++);
  }

  if( wire->endTransmission( true))
    return -1;

  return 0;
}
