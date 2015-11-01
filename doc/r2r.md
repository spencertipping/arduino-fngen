# R-2R DAC design
I'm using 1% resistors, which means that we expect >1 bit of error in a 7-bit
DAC. To fix this, six of the connecting stages can be calibrated using trimmer
resistors (TR1-TRC in the [board diagram](layout.md)).

Arduino pins can source a maximum of 40mA each, but this DAC is designed to
consume a maximum of 10mA per pin, so the base 2R value needs to be above 500Ω;
I'm using 680Ω to be on the safe side (especially since we're also powering an
LCD module). Here's the resulting topology:

```
            0A          1A              4A          5A          6A
             |           |               |           |           |
            680         680             680         680         680
             |           |               |           |           |
GND --680-- R0A --340-- R1A --340-- ... R4A --340-- R5A --TRB-- R6A --TR9-- ...
```

This design requires 13 680Ω and 5 340Ω resistors per channel, which is a
problem because I only have 20 of each value. However, we can increase the
resistors on the low end with little or no impact on accuracy:

```
            0A          1A          2A          3A
             |           |           |           |
            820         820         680         680
             |           |           |           |
GND --820-- R0A --410-- R1A --270-- R2A --340-- R3A ...

                         ^           ^
                         |           |
                         |           340Ω impedance to ground
                         |
                         410Ω impedance to ground
```

Now each channel requires 10 680Ω, 5 820Ω, 1 270Ω, and 3 340Ω:

```
     0A  1A  2A  3A  4A  5A  6A  7A  8A  9A  AA  BA
    820 820 680 680 680 680 680 680 680 680 680 680
820 410 270 340 340 340 340 TRB TR9 TR7 TR5 TR3 TR1 -> output
```

I'm using three different trimmer values, 500Ω, 1kΩ, and 2kΩ, because I've only
got five of each. The lower values are on the high bits (i.e. `TR1` and `TR3`)
because that's where finer precision is more important.
