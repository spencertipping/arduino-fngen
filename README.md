# Programmable Arduino signal generator
Useful for arbitrary low-frequency signals -- anything up to about 60 kHz. (I'm
also working on a Linux/VGA-based signal generator for higher frequencies, so
stay tuned.) Built for the Arduino MEGA 2560 R3 with an LCD/keypad shield and a
dual 12-bit R-2R DAC described below. The board has six voltage divider pots
to provide quick adjustment and twelve trimmers to calibrate the six
most-significant bits within each channel. (See the [hardware
description](doc/r2r.md) for details.)

## Output limits
All output is filtered through an LM358 op-amp, which has a 0.3V/μs slew rate
per output. At 5Vpp, this comes out to about 16.6μs to slew a full cycle = 60
kHz.

## Timing and analog input
`analogRead()` is right out because using it introduces a 100 μs flatline in
the output signal. Underneath `analogRead`, however, is an [asynchronous
interface](https://github.com/arduino/Arduino/blob/e5252e303141c063d59f4f8c44cb75d738606bce/hardware/arduino/avr/cores/arduino/wiring_analog.c#L78)
that we can use without blocking much of anything. (We need analog reads to
interface with the buttons on the LCD panel, as well as to do on-the-fly
updates when the adjustment pots are turned.)

All timing is relative to the Arduino clock, which we can access using the
timer registers. We configure `timer3` at 16 MHz and `timer4` at 16 kHz to
track the clock-cycle time per iteration. The goal here is to emit an exact
frequency over the long run, which we do by maintaining a monotonic 64-bit
clock.

## R-2R DAC stages and pinout
[Arduino source with the mapping](https://github.com/arduino/Arduino/blob/e5252e303141c063d59f4f8c44cb75d738606bce/hardware/arduino/avr/variants/mega/pins_arduino.h#L136)

We're only guaranteed atomic writes within a single port, so the most useful
configuration is two 12-bit DACs wired like this (which guarantees that the
channels have no relative time offset):

```
# note: PORTA has a reversed bit/pin mapping
PORTA: pins 29-26 -> DAC 1 bits 11-8
       pins 25-22 -> DAC 2 bits 11-8

PORTC: pins 30-33 -> DAC 1 bits 7-4
       pins 34-37 -> DAC 2 bits 7-4

PORTL: pins 42-45 -> DAC 1 bits 3-0
       pins 46-49 -> DAC 2 bits 3-0
```
