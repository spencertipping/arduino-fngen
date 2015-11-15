# Programmable Arduino signal generator
Useful for arbitrary low-frequency signals -- anything up to a few kHz. (I'm
also working on a Linux/VGA-based signal generator for higher frequencies, so
stay tuned.) Built for the Arduino MEGA 2560 R3 with an LCD/keypad shield and a
dual 12-bit R-2R DAC [described here](doc/r2r.md). The board has six voltage
divider pots to provide quick adjustment and twelve trimmers to calibrate the
six most-significant bits within each channel.

Hardware v1:

![](http://spencertipping.com/arduino-fngen-hardware-v1.jpg)

2-channel output with prototype wavetable generator:

![](http://spencertipping.com/arduino-fngen-hardware-v1-output.jpg)

## Signal properties
This signal generator prefers variance to bias; that is, over the long run
you'll get almost exactly the signal you ask for. In particular, several
aspects of signals will be long-run correct:

1. Signal level/voltage will converge to 0.04%
2. Frequency will converge to ±10ppm
3. Cross-channel error covariance is very close to 100%
4. Cross-channel timing delta is <<60ns, probably unmeasurably small (it's the
   maximum pin/pin timing difference within a single-instruction 8-bit Arduino
   port write, plus whatever electrical delays exist in the DAC)

As a result of (3) and (4), two opposite waveforms should almost exactly
cancel.

## Signal generation strategy
We can't use a lookup table because the Arduino has very little memory; at a
2MHz downscaled clock rate and 24 bits per sample, 8kB provides a maximum
window of just over 1ms. Extending this window involves using an event bytecode
with the following properties:

- Commands are maintained in a time-sorted priority queue
- Two commands exist, level-set and slope-set
- Realtime pauses are inversely proportional to slope of signal being emitted

The bit-mixing stage alone looks like this:

```cpp
uint16_t sig1, sig2;
uint8_t a = (sig1 & 0x0f00) >> 4 | (sig2 & 0x0f00) >> 8;
uint8_t c = (sig1 & 0x00f0) >> 0 | (sig2 & 0x00f0) >> 4;
uint8_t l = (sig1 & 0x000f) << 4 | (sig2 & 0x000f) << 0;
PORTA = a;
PORTC = c;
PORTL = l;
```

An optimal compilation here uses 15 single-clock AVR instructions, for a ~1MS/s
clock rate maximum:

```
6x byte AND
3x byte left/right shifts
3x byte OR
3x byte move to port registers
```

We're actually at liberty to drop some of the register writes for high-speed
signals, reducing the overhead to 5 instructions, or a ~3MS/s clock rate. In
practice it will be slower because the signal needs to be retrieved at a cost
of at least two more instructions (regardless of the number of ports used to
emit it).
