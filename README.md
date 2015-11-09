# Programmable Arduino signal generator
Useful for arbitrary low-frequency signals -- anything up to a few kHz. (I'm
also working on a Linux/VGA-based signal generator for higher frequencies, so
stay tuned.) Built for the Arduino MEGA 2560 R3 with an LCD/keypad shield and a
dual 12-bit R-2R DAC described below. The board has six voltage divider pots
to provide quick adjustment and twelve trimmers to calibrate the six
most-significant bits within each channel. (See the [hardware
description](doc/r2r.md) for details.)
