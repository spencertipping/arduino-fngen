#ifndef ASYNC_ANALOG_H
#define ASYNC_ANALOG_H

uint16_t *async_analog_dest = 0;

#define async_analog_ready() (!async_analog_dest)

#define async_analog_check()                                            \
  do                                                                    \
  {                                                                     \
    if (!bit_is_set(ADCSRA, ADSC) && async_analog_dest)                 \
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

  bool has_changed()
  {
    async_analog_check();
    bool result = value >> 2 != last_value;
    last_value  = value >> 2;
    return result;
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

#define LCD_POT 6

AnalogMonitor pots[] = { AnalogMonitor(9),
                         AnalogMonitor(10),
                         AnalogMonitor(11),
                         AnalogMonitor(12),
                         AnalogMonitor(13),
                         AnalogMonitor(14),
                         AnalogMonitor(0) };

#endif
