# Programmable Arduino signal generator
Useful for arbitrary low-frequency signals -- anything up to a few kHz. (I'm
also working on a Linux/VGA-based signal generator for higher frequencies, so
stay tuned.) Built for the Arduino MEGA 2560 R3 with an LCD/keypad shield and a
dual 12-bit R-2R DAC described below. The board has six voltage divider pots
to provide quick adjustment and twelve trimmers to calibrate the six
most-significant bits within each channel. (See the [hardware
description](doc/r2r.md) for details.)

## Signal generation strategy
It takes the Arduino a long time to generate signals, even if they're stored in
a lookup table. Buffering will work, but since we have only 8kb of memory we
can't store more than ~2k 12-bit points for each channel. This, in turn, means
that we need to carefully manage the buffer size.

### Signal elements
Signals are sums, products, or compositions of square, sawtooth, sine,
constant, or potentiometer elements. Potentiometers are updated in real-time.
Each of these elements is identified by a single character:

- `s`: sine wave
- `q`: square wave
- `w`: sawtooth wave
- `A`-`F`: potentiometer

Signals are managed on a stack and combined with one of the following:

- `.`: compose
- `+`: add
- `*`: multiply

There are also some stack commands:

- `%`: duplicate
- `!`: drop
- `/`: swap

### Examples
- `ws.`: swept-frequency 0-1Hz sine wave
- `qs*`: 1Hz sine wave modulated by 1Hz square wave

## Timing and analog input
`analogRead()` is right out because using it introduces a 100 μs flatline in
the output signal. Underneath `analogRead`, however, is an [asynchronous
interface](https://github.com/arduino/Arduino/blob/e5252e303141c063d59f4f8c44cb75d738606bce/hardware/arduino/avr/cores/arduino/wiring_analog.c#L78)
that we can use without blocking much of anything. (We need analog reads to
interface with the buttons on the LCD panel, as well as to do on-the-fly
updates when the adjustment pots are turned.)

## R-2R DAC stages and pinout
[Arduino source with the mapping](https://github.com/arduino/Arduino/blob/e5252e303141c063d59f4f8c44cb75d738606bce/hardware/arduino/avr/variants/mega/pins_arduino.h#L136)

We're only guaranteed atomic writes within a single port, so the most useful
configuration is two 12-bit DACs wired like this (which guarantees that the
channels have no relative time offset):

```
# note: PORTA has a reversed bit/pin mapping that's fixed in hardware
PORTA: pins 29-26 -> DAC 1 bits 11-8
       pins 25-22 -> DAC 2 bits 11-8

PORTC: pins 30-33 -> DAC 1 bits 7-4
       pins 34-37 -> DAC 2 bits 7-4

PORTL: pins 42-45 -> DAC 1 bits 3-0
       pins 46-49 -> DAC 2 bits 3-0
```
