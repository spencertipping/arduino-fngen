#ifndef SHAPES_H
#define SHAPES_H

class shape
{
 public:
  virtual uint16_t max_dxdt()    = 0;
  virtual uint32_t periodicity() = 0;
};

class square_wave : public shape
{
 public:
  uint16_t low, high;
  uint32_t don, doff;

  square_wave(uint32_t _dt) : don(_dt >> 1), doff(_dt >> 1),
                              low(0), high(0xffff) {}
  square_wave(uint32_t _don, uint32_t _doff) : don(_don), doff(_doff),
                                               low(0), high(0xffff) {}
  square_wave(uint32_t _don, uint32_t _doff, uint16_t _low, uint16_t _high)
    : don(_don), doff(_doff), low(_low), high(_high) {}

  virtual uint16_t max_dxdt()    { return 0xffff; }
  virtual uint32_t periodicity() { return don + doff; }
};

#endif
