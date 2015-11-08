#ifndef ASYNC_ANALOG_H
#define ASYNC_ANALOG_H

#include "emit.hh"

uint16_t *async_analog_dest = 0;

#define async_analog_ready() (!async_analog_dest)

#define async_analog_check()                                            \
  do                                                                    \
  {                                                                     \
    if (async_analog_dest && !bit_is_set(ADCSRA, ADSC))                 \
    {                                                                   \
      uint8_t low, high;                                                \
      low  = ADCL;                                                      \
      high = ADCH;                                                      \
      *async_analog_dest = high << 8 | low;                             \
      async_analog_dest = 0;                                            \
    }                                                                   \
  } while (0)

#define async_analog_read(pin, into)                                    \
  do                                                                    \
  {                                                                     \
    if (async_analog_ready())                                           \
    {                                                                   \
      async_analog_dest = (into);                                       \
      ADMUX   = DEFAULT << 6 | (pin) & 7;                               \
      ADCSRB  = ((pin) >> 3 & 1) << MUX5;                               \
      ADCSRA |= 1 << ADSC;                                              \
    }                                                                   \
  } while (0)

class AnalogMonitor
{
 private:
  uint8_t  pin;
  uint8_t  last_value;
  uint16_t value;

 public:
  AnalogMonitor(uint8_t _pin): pin(_pin), last_value(0), value(0) {}

  void reset()
  {
    last_value = value >> 2;
  }

  void sync()
  {
    update();
    while (!async_analog_ready())
    {
      emit();
      async_analog_check();
    }
    reset();
  }

  char delta(int shift)
  {
    async_analog_check();
    return ((char) (value >> 2) - (char) last_value) + (1 << shift - 1)
           >> shift;
  }

  void update()
  {
    async_analog_check();
    async_analog_read(pin, &value);
  }

  uint16_t val()
  {
    async_analog_check();
    return value;
  }
};

#define N_POTS (sizeof(pots) / sizeof(AnalogMonitor))
#define CODE_POT 1
#define MOVE_POT 2
#define LCD_POT  6

AnalogMonitor pots[] = { AnalogMonitor(9),
                         AnalogMonitor(10),
                         AnalogMonitor(11),
                         AnalogMonitor(12),
                         AnalogMonitor(13),
                         AnalogMonitor(14),
                         AnalogMonitor(0) };

bool any_pot_delta = false;
char which_pot     = 0;

#define check_pot()                                             \
  do {                                                          \
    if (++which_pot >= N_POTS) which_pot = 0;                   \
    pots[which_pot].update();                                   \
    any_pot_delta |= pots[which_pot].delta(5);                  \
  } while (0)

#endif
