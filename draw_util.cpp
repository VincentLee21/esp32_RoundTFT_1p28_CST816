#include "draw_util.h"
#include "DEV_Debug.h"

uint32_t u16_to_rgb32(uint16_t value) {
  uint32_t out = 0;
  uint8_t red   = 0; // Red is the top 5 bits of a 16-bit colour value
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  red   = (value >> 11) & 0x1F;
  green = (value >> 5) & 0x3F;
  blue  = (value >> 0) & 0x1F;

  out = red << 16 | green << 8 | blue;
  return out;
}

uint32_t u16_to_rgb32_fullrange(uint16_t value) {
  uint32_t out = 0;
  uint8_t red   = 0; // Red is the top 5 bits of a 16-bit colour value
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  out = u16_to_rgb32(value);
  red   = (((out >> 16) & 0xFF) * 8);
  green = (((out >>  8) & 0xFF) * 4);
  blue  = (((out >>  0) & 0xFF) * 8);

  red = red > 30 ? red - 30 : red;
  green = green > 30 ? green - 30 : green;
  blue = blue > 30 ? blue - 30 : blue;

  out = red << 16 | green << 8 | blue;
  return out;
}

uint16_t rgb_to_u16(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t rgb = 0;
  uint8_t red   = 0; // Red is the top 5 bits of a 16-bit colour value
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  red   = r & 0x1F;
  green = g & 0x3F;
  blue  = b & 0x1F;

  rgb = (red<<11) | (green<<5) | blue;
  return rgb;
}

uint16_t rgb32_to_u16(uint32_t rgb) {
  uint16_t rgb16 = 0;
  uint8_t red   = 0; // Red is the top 5 bits of a 16-bit colour value
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  red   = (rgb >> 16) & 0x1F;
  green = (rgb >> 8 ) & 0x3F;
  blue  = (rgb >> 0 ) & 0x1F;

  rgb16 = (red<<11) | (green<<5) | blue;
  return rgb16;
}
