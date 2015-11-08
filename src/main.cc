#include <LiquidCrystal.h>

#include "async-analog.hh"
#include "emit.hh"
#include "interpreter.hh"
#include "wavetables.hh"
#include "lcd.hh"
#include "repeat.hh"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LCDKey        lcdkey;

program p;
int edit_point = 0;
bool run_mode  = false;

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
  approx_stage(7, 5);
  approx_stage(5, 3);
  approx_stage(3, 1);

  for (int i = 0; i < BUF_SIZE && !any_pot_delta; ++i)
  {
    check_pot();
    recompile_one(i);
  }

#undef recompile_one
}

void update_display()
{
  lcd.home();
  lcd.clear();

  for (int i = 0; i < PROGRAM_SIZE / 2; ++i)
    lcd.print(commands[p.code[i]]);
  lcd.setCursor(0, 1);
  for (int i = 0; i < PROGRAM_SIZE / 2; ++i)
    lcd.print(commands[p.code[i + PROGRAM_SIZE / 2]]);
  lcd.setCursor(edit_point & 0x0f, edit_point >> 4);

  if (run_mode) lcd.noCursor();
  else          lcd.cursor();
}

void setup()
{
  DDRL = DDRC = DDRA = 0xff;

  lcd.begin(16, 2);
  Serial.begin(9600);

  for (int i = 0; i < PROGRAM_SIZE; ++i) p.code[i] = 0;
  p.set("b*^: s / c+s");

  TCCR3B = 0x0a;
  TCCR3A = 0x00;
  OCR3A  = BUF_SIZE;

  for (int i = 0; i < N_POTS; ++i) pots[i].sync();
  while (1)
  {
    check_pot();

    if (run_mode)
    {
      int cmd = lcdkey.command(pots[LCD_POT].val());
      if (cmd == LCDSelect)    run_mode = false;
      else if (cmd != LCDNone) recompile();
      else
      {
        for (int i = 0; i < 128; ++i)
        {
          repeat_32( emit(); )
        }

        if (any_pot_delta)
        {
          any_pot_delta = false;
          for (int i = 0; i < N_POTS; ++i) pots[i].sync();
          recompile();
        }
      }
    }
    else
    {
      sei();
      PORTA = PORTC = PORTL = 0;

      if (Serial.available())
      {
        int i;
        char buf[PROGRAM_SIZE + 1];
        char c = 0;
        for (i = 0; (c = Serial.read()) >= ' ' && i < PROGRAM_SIZE; ++i)
          buf[i] = c;
        buf[i] = 0;
        p.set(buf);
        recompile();
        run_mode = true;
        update_display();
      }
      else
      {
        char move   = pots[MOVE_POT].delta(4);
        char diff   = pots[CODE_POT].delta(4);
        bool update = false;

        switch (lcdkey.command(pots[LCD_POT].val()))
        {
          case LCDRight:  update = true; ++edit_point;         break;
          case LCDLeft:   update = true; --edit_point;         break;
          case LCDUp:     update = true; ++p.code[edit_point]; break;
          case LCDDown:   update = true; --p.code[edit_point]; break;
          case LCDSelect: update = true; run_mode = true;      break;
          case LCDNone:
            if (move)
            {
              edit_point += move;
              pots[MOVE_POT].reset();
              update = true;
            }
            else if (diff)
            {
              p.code[edit_point] += diff;
              pots[CODE_POT].reset();
              update = true;
            }
            break;
        }

        edit_point &= PROGRAM_SIZE - 1;
        p.code[edit_point] += command_len;
        p.code[edit_point] %= command_len;

        if (run_mode)
        {
          recompile();
          for (int i = 0; i < N_POTS; ++i) pots[i].reset();
          cli();
        }

        if (update) update_display();
      }
    }
  }
}

void loop()
{}
