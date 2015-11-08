#ifndef WAVETABLES_H
#define WAVETABLES_H

static uint16_t const sine_table[] PROGMEM = {
#perl printf "%f,", sin($_ / 2048 * PI) for 0..4096;
};

#endif
