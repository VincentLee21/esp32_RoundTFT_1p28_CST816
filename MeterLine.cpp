#include "MeterLine.h"
#include "DEV_Debug.h"

// -----------------------------------
// drawLineMeter
// -----------------------------------
drawLineMeter::drawLineMeter() {
  _x = 0;
  _y = 0;
  _height = 20;
  _width = 8;
  _vertical = 0;
  _startfromTL = 0;
  _ptrImg = NULL;
  _gradient = false;
  _fg1_color = TFT_BLACK;
  _fg2_color = TFT_BLACK;
  _font_color = TFT_BLACK;
  _bg_color = TFT_BLACK;
}

drawLineMeter::~drawLineMeter() {
}

void drawLineMeter::Initialize( char *name, char *appendString, TFT_eSprite *ptrImg,
                                      int height, int width, int outline_width, bool vertical,
                                      bool gradient, float min_val, float max_val,
                                      uint32_t fg1_color, uint32_t fg2_color,
                                      uint32_t bar_bg_color, uint32_t bg_color) {

  float fR, fG, fB;
  float fdR, fdG, fdB;

  _height = height;
  _width = width;
  _outlinewidth = outline_width;
  _vertical = vertical;
  // _startfromTL = fromTopLeft;
  _ptrImg = ptrImg;
  _minval = min_val;
  _maxval = max_val;
  _range = (max_val - min_val);
  _color_step_size = ceil((max_val-min_val)/10);

  _ovrf = false;
  _undf = false;

  if( _vertical) {
    _step_size = float(height)/_range;
    _txtx = width + (outline_width *2) + 5;
    _txty = int( float(height)*0.5) + 5;
  } else {
    _step_size = float(height)/_range;
    _txtx = int( float(height)*0.5) + 5;
    _txty = width + (outline_width *2) + 5;
  }

  // _val_angle = ;
  _fg1_color = fg1_color;
  _fg2_color = fg2_color;
  _bar_bg_color = bar_bg_color;
  _bg_color = bg_color;
  _gradient = gradient;
  _last_bar_color = 0;

  if( name == NULL)
    sprintf( _name, "NULL");
  else
    strcpy( _name, name);

  if(( appendString == NULL) || (strlen(appendString) > 4))
    sprintf( _appendString, "-");
  else
    strcpy( _appendString, appendString);

  if( _minval < 0.0)
    _offset = _minval * -1;

  if( gradient) {
    uint32_t tmpColor1, tmpColor2;
    uint8_t r1, g1, b1;
    uint8_t r2, g2, b2;
    int dr, dg, db;

    tmpColor1 = u16_to_rgb32(fg1_color);
    r1 = (tmpColor1 >> 16) & 0xFF;
    g1 = (tmpColor1 >> 8) & 0xFF;
    b1 = (tmpColor1 >> 0) & 0xFF;
    tmpColor2 = u16_to_rgb32(fg2_color);
    r2 = (tmpColor2 >> 16) & 0xFF;
    g2 = (tmpColor2 >> 8) & 0xFF;
    b2 = (tmpColor2 >> 0) & 0xFF;
    fdR = float(r2 - r1) *0.1;
    fdG = float(g2 - g1) *0.1;
    fdB = float(b2 - b1) *0.1;

    fR = (float)r1;
    fG = (float)g1;
    fB = (float)b1;

    for( int i = 0; i < 10; i++) {
      r2 = int(fR);
      g2 = int(fG);
      b2 = int(fB);
      tmpColor2 = rgb_to_u16(r2, g2, b2);
      _colorArry[i] = tmpColor2 ;
      // Debug("_colorArry[%d] %d, %d, %d\n", i, r2, g2, b2);
      fR += fdR;
      fG += fdG;
      fB += fdB;
    }
  }
  Debug("%s : _minval %3.2f _maxval %3.2f _step_size %3.3f\n", _name, _minval, _maxval,  _step_size);
  Debug("%s : _width %d, _height %d\n", _name, _width, _height);
  Debug("%s : _offset %3.3f\n", _name, _offset);
  Debug("%s : _color_step_size %d\n", _name, _color_step_size);
}

void drawLineMeter::SetLabelOrig(int x, int y) {
  _txtx = x;
  _txty = y;
}

void drawLineMeter::_intDraw( float value, uint32_t font_color) {
  int i, last_one;
  int segment_start, segment_width, tmp_height, tmp_width, tmp_outl_height, tmp_outl_width;
  int _val, total_length_ref, tmp_a;
  float value_wi_offset;
  uint16_t oldColor;
  uint16_t newColor;

  _x = _outlinewidth;
  _y = _outlinewidth;

  if( _selftest)
    _value = _tstval;
  else
    _value = value;

  value_wi_offset = _value + _offset;  // start from 0

  _ovrf = false;
  _undf = false;

  // limited the value
  if( _value > _maxval) {
    value_wi_offset = _maxval + _offset;
    _ovrf = true;
  } else  if( _value <= _minval) {
    value_wi_offset = _minval + _offset + 1;
    _undf = true;
  }
  // limited the value

  _val = int(value_wi_offset*_step_size);
  if( _val < (_outlinewidth<<1))
    _val = _outlinewidth<<1;

  _ptrImg->fillScreen(_bg_color);
  // _ptrImg->fillScreen(TFT_RED); // debug

  // Draw red box
  if( _selftest) {
    _img_w = _ptrImg->width();
    _img_h = _ptrImg->height();
    _ptrImg->drawRect( 0, 0, _img_w, _img_h, TFT_RED);
  }

  if( _vertical == false) {
    tmp_height = _width;
    tmp_width  = _height;
    tmp_outl_width = _height + _outlinewidth;
    tmp_outl_height = _width + (_outlinewidth*2);
  } else {
    tmp_height = _height;
    tmp_width  = _width;
    tmp_outl_width = _width + (_outlinewidth*2);
    tmp_outl_height = _height + (_outlinewidth);
  }

  _ptrImg->fillSmoothRoundRect( 0, 0, // x,  y,
                     tmp_outl_width, tmp_outl_height,  //  w,  h
                     10,  // radius,
                     _bar_bg_color, _bg_color);  // fg_color, bg_color);

  // Debug( "%s : value_wi_offset %3.1f, _val %d\n", _name, value_wi_offset, _val);

  if( _gradient) {
    oldColor = _bg_color;
    segment_start = _x;

    int step2 = _color_step_size * _step_size;
    last_one = 0;

    total_length_ref = _val; // total length of bar
    int vert_segment_start = tmp_height;

    for( int ii = 0; ii < 10; ii++) {
      segment_width = step2;
      newColor = _colorArry[ii];

      // Debug( "%s : 0 - ii %d, total_length_ref %d, segment_start %d, segment_width %d\n",
      //       _name, ii, total_length_ref, segment_start, segment_width);

      if( total_length_ref <= step2) {
        segment_width = step2 - segment_start;
        last_one = 1;
        // Debug( "%s : 1 - ii %d, total_length_ref %d, segment_start %d, segment_width %d\n",
        //       _name, ii, total_length_ref, segment_start, segment_width);
      } else {
        if( ii == 0)
          segment_width = segment_width - segment_start; // adjust for first segment

        if(( segment_start+segment_width) > total_length_ref) {
          // Debug( "%s : 2 - ii %d, total_length_ref %d, segment_start %d, segment_width %d\n",
          //       _name, ii, total_length_ref, segment_start, segment_width);
          segment_width = total_length_ref - segment_start;
          last_one = 1;
          // Debug( "%s : 3 - new segment_width %d\n",
          //       _name, segment_width);
        }
      }

      if( _vertical) {
        if( (ii == 0) || (ii == 9) || last_one) {

          vert_segment_start -= segment_width;

          // Debug( "%s : ii %d, vert_segment_start %d, segment_width %d, last_one %d\n",
          //       _name, ii, vert_segment_start, segment_width, last_one);

          if( ii == 0)
            vert_segment_start -= 1; // increase overlap area for next segment
          else if( ii == 9)
            vert_segment_start += 1; // increase overlap area for next segment

          _ptrImg->fillSmoothRoundRect( _x, vert_segment_start, //x, y,
                             tmp_width, segment_width+1,  // w, h
                             3,  // radius,
                             newColor, oldColor);  // fg_color, bg_color);

        } else {
          vert_segment_start -= segment_width ;

          // Debug( "%s : ii %d, vert_segment_start %d, segment_width %d, last_one %d\n",
          //       _name, ii, vert_segment_start, segment_width, last_one);

          _ptrImg->fillRect( _x, vert_segment_start, // x,  y,
                             tmp_width, segment_width+1,  //  w,  h
                             newColor);  // fg_color, bg_color);
        }
      }
      else {  // horizontal
        if( (ii == 0) || (ii == 9) || last_one) {
          segment_start = (ii == 9) ? segment_start -1 : segment_start; // increase overlap area for next segment
          segment_width = (ii == 0) ? segment_width + 1 : segment_width; // increase overlap area for next segment
          _ptrImg->fillSmoothRoundRect( segment_start, _y, //x, y,
                             segment_width, tmp_height,  // w, h
                             3,  // radius,
                             newColor, oldColor);  // fg_color, bg_color);
          segment_width = (ii == 0) ? segment_width - 1 : segment_width; // restore value
        } else {
          _ptrImg->fillRect( segment_start, _y, // x,  y,
                             segment_width, tmp_height,  //  w,  h
                             newColor);  // fg_color, bg_color);
        }
      }
      segment_start += segment_width;

      oldColor = newColor;
      _last_bar_color = newColor;
      if( last_one) {
        break;
      }
    }

  } else {

    if( _vertical) {
      tmp_height = _height - _val + _outlinewidth; // start from top side
      _ptrImg->fillSmoothRoundRect( _x, tmp_height, // x,  y,
                         tmp_width, _val - _outlinewidth,  //  w,  h
                         3,  // radius,
                         _fg1_color, _bg_color);  // fg_color, bg_color);
    } else {
      tmp_width = _val;
      _ptrImg->fillSmoothRoundRect( _x, _y, // x,  y,
                         tmp_width - _outlinewidth, tmp_height,  // w, h
                         3,  // radius,
                         _fg1_color, _bg_color);  // fg_color, bg_color);
    }
    _last_bar_color = _fg1_color;
  }

  if( _selftest) {

    _tstval += 0.45;

    if( _tstval > (_maxval+_tstextra))
      _tstval = _minval - _tstextra;
    // _tstval = 100;
    // _tstval = -5;
  }
}

// ----------------------------------------
// Draw meter
// ----------------------------------------
void drawLineMeter::Draw( float value, uint32_t font_color) {
  String localString;
  float localvalue = getValue();

  _intDraw(value, font_color);

  localString = String( localvalue, 1) + String( _appendString);

  if( _vertical)
    _ptrImg->setTextDatum( ML_DATUM);
  else
    _ptrImg->setTextDatum( TC_DATUM);
  _ptrImg->setTextColor( font_color, _bg_color, true);

  #ifdef GFXFF
  _ptrImg->setFreeFont( GFXFF_12);
  _ptrImg->drawString(localString, _txtx, _txty, GFXFF);
  #else
  _ptrImg->setFreeFont( NULL);
  _ptrImg->drawString(localString, text_x, text_y, 4);
  #endif
}

void drawLineMeter::Draw( float value, uint32_t font_color, String string) {
  String localString;
  float localvalue = getValue();

  _intDraw(value, font_color);

  if( string.length() > 0)
    localString = string;
  else
    localString = String( localvalue, 1) + String( _appendString);

  if( _vertical)
    _ptrImg->setTextDatum( ML_DATUM);
  else
    _ptrImg->setTextDatum( TC_DATUM);
  _ptrImg->setTextColor( font_color, _bg_color, true);

  #ifdef GFXFF
  _ptrImg->setFreeFont( GFXFF_12);
  _ptrImg->drawString(localString, _txtx, _txty, GFXFF);
  #else
  _ptrImg->setFreeFont( NULL);
  _ptrImg->drawString(localString, text_x, text_y, 4);
  #endif
}

// ----------------------------------------
// Get current value
// ----------------------------------------
float drawLineMeter::getValue(void) {
  return _value;
}

// ----------------------------------------
// Get color value of the value
// output 0x00, 8bits-R, 8bits-G, 8bits-B
// ----------------------------------------
uint32_t drawLineMeter::getBarColor(void) {
  uint32_t ret = 0;
  ret = u16_to_rgb32_fullrange(_last_bar_color);
  return ret;
}

// ----------------------------------------
// Enable self test
// start from min value -5
// to max value +5
// ----------------------------------------
void drawLineMeter::SelfTestEnable(void) {
  _tstval = _minval - _tstextra;
  _selftest = true;
}

// ----------------------------------------
// Disable self test
// ----------------------------------------
void drawLineMeter::SelfTestDisable(void) {
  _selftest = false;
}

