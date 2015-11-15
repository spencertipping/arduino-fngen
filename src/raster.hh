#ifndef RASTER_H
#define RASTER_H

#include <avr/progmem.h>

// Raster letter/symbol renditions useful for user interfaces for XY-mode
// oscilloscopes. Each symbol is rendered within an 8x4-pixel space, which
// greatly reduces the number of port-register writes that need to happen. We
// store them as 32-bit bitmaps and skip over zero bits.

// First entry is ASCII 32
uint32_t raster_characters[] PROGMEM = {
  0x00000000,
  0x02220020,           // !
  0x09900000,           // "
  0x05f55f50,           // #
  0x4f5f75f4,           // $
  0x0cc00cc0,           // %
  0xffffffff,           // &
  0x02200000,           // '
  0x22444422,           // (
  0x44222244,           // )
  0x045f4000,           // *
  0x0044f440,           // +
  0x00000046,           // ,
  0x0000f000,           // -
  0x00000066,           // .
  0x00124800,           // /
  0x069bd960,           // 0
  0x04644470,           // 1
};

void render_raster_char(char c, int x, int y)
{
  
  for (int i = 0; i < 32; ++i)
    if (c >> i & 1)
      
}

#endif
