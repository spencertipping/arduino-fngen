#include <LiquidCrystal.h>

#include "async-analog.hh"
#include "interpreter.hh"
#include "wavetables.hh"
#include "lcd.hh"
#include "repeat.hh"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LCDKey        lcdkey;

program p;
int edit_point = 0;

bool run_mode = false;

void update_display()
{
  lcd.home();
  lcd.clear();

  if (run_mode)
    lcd.print("running");
  else
  {
    for (int i = 0; i < PROGRAM_SIZE; ++i)
      lcd.print(commands[p.code[i]]);
    lcd.setCursor(edit_point & 0x1f, !!(edit_point & 0x20));
    lcd.cursor();
  }
}

#define BUF_SIZE 2048

void setup()
{
  cli();
  DDRL = DDRC = DDRA = 0xff;

  char which_pot = 0;

  for (int i = 0; i < PROGRAM_SIZE; ++i) p.code[i] = 0;
  p.set("s=");

  TCCR3B = 0x0a;
  TCCR3A = 0x00;
  OCR3A  = BUF_SIZE;

  lcd.begin(16, 2);

  uint8_t a_buf[BUF_SIZE];
  uint8_t c_buf[BUF_SIZE];
  uint8_t l_buf[BUF_SIZE];

  while (1)
  {
    if (run_mode)
    {
      pots[LCD_POT].update();
      run_mode &= lcdkey.command(pots[LCD_POT].val()) == LCDNone;

      int t;
      uint8_t a, c, l;
      for (uint8_t i = 1; i; ++i)
      {
        repeat_32(
          t = TCNT3;
          a = a_buf[t];
          c = c_buf[t];
          l = l_buf[t];
          PORTA = a;
          PORTC = c;
          PORTL = l;
        )
      }
    }
    else
    {
      if (++which_pot >= sizeof(pots) / sizeof(AnalogMonitor))
        which_pot = 0;

      pots[which_pot].update();
      if (pots[which_pot].has_changed())
      {
        PORTA = PORTC = PORTL = 0;

        switch (lcdkey.command(pots[LCD_POT].val()))
        {
          case LCDNone:                        break;
          case LCDRight: ++edit_point;         break;
          case LCDLeft:  --edit_point;         break;
          case LCDUp:    ++p.code[edit_point]; break;
          case LCDDown:  --p.code[edit_point]; break;
          case LCDSelect: run_mode = true;     break;
        }

        edit_point &= PROGRAM_SIZE - 1;
        p.code[edit_point] += command_len;
        p.code[edit_point] %= command_len;

        if (run_mode)
        {
          stack s;
          for (int i = 0; i < BUF_SIZE; ++i)
          {
            if (!(i & 0xff))
            {
              lcd.home();
              lcd.clear();
              lcd.print(i);
              lcd.print(" / ");
              lcd.print(BUF_SIZE);
            }

            s.init1(i);
            interpret(&p, &s);
            uint16_t ca = s.pop();
            uint16_t cb = s.pop();
            a_buf[i] = ca & 0xf000 | cb >> 4 & 0xf000;
            c_buf[i] = ca & 0x0f00 | cb >> 4 & 0x0f00;
            l_buf[i] = ca & 0x00f0 | cb >> 4 & 0x00f0;
            OCR3A = i;
          }
        }

        update_display();
      }
    }
  }
}

void loop()
{}
