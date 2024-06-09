#ifndef __METERLINE_H__
#define __METERLINE_H__

#include "Arduino.h"
#include <TFT_eSPI.h>
#include "draw_util.h"

class drawLineMeter {
  private:
  TFT_eSprite *_ptrImg;
  int         _x, _y;         // meter coordinate
  int         _txtx, _txty;   // label coordinate
  int         _height;        // meter height
  int         _width;         // meter width
  int         _outlinewidth;  // shell width
  int         _range;         // max - min value
  int         _color_step_size; // color bar step size
  int         _img_w, _img_h;
  float       _minval, _maxval;
  float       _offset, _value, _tstval;
  float       _step_size;
  const float _tstextra = 5;
  uint16_t    _fg1_color;     // bar color 1
  uint16_t    _fg2_color;     // bar color 2
  uint16_t    _font_color;    // font color
  uint16_t    _bar_bg_color;
  uint16_t    _bg_color, _last_bar_color;
  uint16_t    _colorArry[10];   // color for gradient mode
  char        _name[16];        // for debug
  char        _appendString[4]; // append string
  bool        _vertical;        // vertical line mode
  bool        _startfromTL;     // starting from
  bool        _gradient;        // gradient mode
  bool        _ovrf;            // input value overflow
  bool        _undf;            // input value underflow
  bool        _selftest;        // self test

  void _intDraw( float value, uint32_t font_color);

  public:
  drawLineMeter(void);
  ~drawLineMeter(void);

  void Initialize(char *name, char *appendString, TFT_eSprite *ptrImg,
                  int height, int width, int outline_width, bool vertical,
                  bool gradient, float min_val, float max_val,
                  uint32_t fg1_color, uint32_t fg2_color,
                  uint32_t bar_bg_color, uint32_t bg_color);

  void SetLabelOrig(int x, int y);
  void Draw( float value, uint32_t font_color, String string);
  void Draw( float value, uint32_t font_color);

  float getValue(void);
  uint32_t getBarColor(void);

  // for test
  void SelfTestEnable(void);
  void SelfTestDisable(void);
};
#endif
