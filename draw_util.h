#ifndef __DRAWUTIL_H__
#define __DRAWUTIL_H__

#include "Arduino.h"
#include <TFT_eSPI.h>
// #include "Fonts/SFNS_12pt8.h"
// #include "Fonts/SFNS_16pt8.h"
// #include "Fonts/Verdana_Bold_12pt8.h"
// #include "Fonts/Arial_R_Bold_12pt8.h"
// #include "Fonts/Arial_R_Bold_14pt8.h"
// #include "Fonts/Arial_R_Bold_16pt8.h"
// #include "Fonts/Arial_R_Bold_20pt8.h"
// #include "Fonts/Arial_Narrow_12pt8.h"
#include "Fonts/Arial_Narrow_16pt8.h"
// #include "Fonts/Arial_Narrow_18pt8.h"


#define GFXFF 1

// #define FF12 &FreeSans12pt7b
// #define FF12 &FreeSerif12pt7b
// #define GFXFF_12 &Arial_R_Bold12pt8b
// #define GFXFF_14 &Arial_R_Bold14pt8b
// #define GFXFF_12 &Arial_R_Bold16pt8b
// #define GFXFF_20 &Arial_R_Bold20pt8b
// #define GFXFF_12 &Arial_Narrow12pt8b
#define GFXFF_12 &Arial_Narrow16pt8b
// #define GFXFF_12 &Arial_Narrow18pt8b

uint32_t u16_to_rgb32(uint16_t value);
uint32_t u16_to_rgb32_fullrange(uint16_t value);
uint16_t rgb_to_u16(uint8_t r,uint8_t g,uint8_t b);
uint16_t rgb32_to_u16(uint32_t rgb);

#endif
