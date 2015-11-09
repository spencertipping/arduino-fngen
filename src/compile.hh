#include "async-analog.hh"
#include "emit.hh"
#include "interpreter.hh"
#include "repeat.hh"

program p;

// Progressively evaluate to get fast approximate results
void recompile()
{
  stack s;
  uint16_t a, b, la, lb;

#define emit_a(i, va, vb)                                       \
  do {                                                          \
    a_buf[i] = ((va) & 0x0f00) >> 4 | (vb & 0x0f00) >> 8;       \
  } while (0)

#define emit_c(i, va, vb)                                       \
  do {                                                          \
    c_buf[i] = (va) & 0x00f0 | (vb & 0x00f0) >> 4;              \
  } while (0)

#define emit_l(i, va, vb)                                       \
  do {                                                          \
    l_buf[i] = ((va) & 0x000f) << 4 | vb & 0x000f;              \
  } while (0)

#define recompile_one(i)                                        \
  do {                                                          \
    s.init1((i) << 1);                                          \
    interpret(&p, &s);                                          \
    la = a;                                                     \
    lb = b;                                                     \
    a = s.pop();                                                \
    b = s.pop();                                                \
    a = (a >> 1) + (a >> 3);                                    \
    b = (b >> 1) + (b >> 3);                                    \
    emit_a(i, a, b);                                            \
    emit_c(i, a, b);                                            \
    emit_l(i, a, b);                                            \
  } while (0)

#define approx_stage(bits, ebits)                               \
  do {                                                          \
    recompile_one(0);                                           \
    for (int i = 1 << bits;                                     \
         i < BUF_SIZE && !any_pot_delta; i += 1 << bits)        \
    {                                                           \
      check_pot();                                              \
      recompile_one(i);                                         \
      for (int j = 1 << ebits; j < 1 << bits; j += 1 << ebits)  \
      {                                                         \
        uint8_t  w  = j << 8 - bits;                            \
        uint16_t va = linear_8(la, a, w);                       \
        uint16_t vb = linear_8(lb, b, w);                       \
        emit_a(i + j - (1 << bits), va, vb);                    \
        emit_c(i + j - (1 << bits), va, vb);                    \
        emit_l(i + j - (1 << bits), va, vb);                    \
      }                                                         \
    }                                                           \
    emit_mask = -1 << ebits;                                    \
  } while (0)

  emit_mask = -1 << 5;
  approx_stage(6, 3);
  approx_stage(3, 1);

  for (int i = 0; i < BUF_SIZE && !any_pot_delta; ++i)
  {
    check_pot();
    recompile_one(i);
  }

#undef recompile_one
}
