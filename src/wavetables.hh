#ifndef WAVETABLES_H
#define WAVETABLES_H

#include <avr/pgmspace.h>

#perl $::wt_bits = 12;
#perl $::wt_size = 1 << $wt_bits;
#perl print "#define WT_BITS $wt_bits"
#perl print "#define WT_SIZE $wt_size"
#perl sub ::wt(&) {printf "%d,", 32768 + 32767 * $_[0]->($_ /= $wt_size) for 0..$wt_size - 1}

#define WT_INTERP (16 - WT_BITS)
#define WT_MASK   (WT_SIZE - 1)

// Quarter-turn sine table, so we mirror and negate accordingly
uint16_t const sine_qtable[] PROGMEM = {
  #perl wt {sin($_ * pip2)}
};

#define linear_8(x, y, w) \
  ((uint32_t) (x) * (255 - (w)) + (uint32_t) (y) * (w) >> 8)

// Linear-interpolation point retrieval from a wavetable
uint16_t wt_linear(uint16_t const *const wt, uint16_t const i)
{
  uint16_t const e1 = i >> WT_INTERP;
  uint16_t const e2 = min(WT_MASK, e1 + 1);
  uint16_t const w1 = i & ~(-1 << WT_INTERP);
  uint16_t const w2 = (1 << WT_INTERP) - w1;
  return (uint32_t) pgm_read_word_near(wt + e1) * w1
       + (uint32_t) pgm_read_word_near(wt + e2) * w2 >> WT_INTERP;
}

uint16_t sine(uint16_t const t)
{
  uint16_t s = wt_linear(sine_qtable, (t & 0x4000 ? ~t & 0x3fff
                                                  :  t & 0x3fff) << 2);
  return t & 0x8000 ? 0xffff - s : s;
}

#define cosine(t) sine((t) + 16384)

#endif
