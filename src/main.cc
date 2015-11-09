#include <LiquidCrystal.h>

#include "async-analog.hh"
#include "compile.hh"
#include "emit.hh"
#include "interpreter.hh"
#include "lcd.hh"
#include "repeat.hh"
#include "shapes.hh"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LCDKey        lcdkey;

int edit_point = 0;
bool run_mode  = false;

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
