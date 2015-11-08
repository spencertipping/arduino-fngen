#ifndef LCD_DETECTOR_H
#define LCD_DETECTOR_H

#include "async-analog.hh"

#define LCDNone 0
#define LCDLeft 1
#define LCDRight 2
#define LCDUp 3
#define LCDDown 4
#define LCDSelect 5

class LCDKey
{
 private:
  bool expecting_key = false;

 public:
  int command(int const val)
  {
    int const a = val >> 6;
    if (!expecting_key) {
      if (a == 15) expecting_key = true;
      return LCDNone;
    }

    switch (a) {
    case 3:  expecting_key = false; return LCDDown;
    case 1:  expecting_key = false; return LCDUp;
    case 6:  expecting_key = false; return LCDLeft;
    case 0:  expecting_key = false; return LCDRight;
    case 10: expecting_key = false; return LCDSelect;
    default:
      expecting_key = true;
      return LCDNone;
    }
  }
};

#endif
