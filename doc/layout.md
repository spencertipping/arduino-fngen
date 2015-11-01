# Board layout
For the second attempt I'm using a 22x27 prototyping board. This has a lot more
room for resistors, and also supports a better layout for the UI components.

## Original design (no way)
**Note:** This is what I had originally intended to do, but gave up at this
point:

![too many resistors](http://spencertipping.com/arduino-fngen-small-board-nope.jpg)

I'm using a 14x20 prototyping board with some headers soldered to match Arduino
pin positions. Here are the large components before soldering (ignore the
trimmer values; at the time this photo was taken I hadn't solved for resistors
yet):

![large components](http://spencertipping.com/arduino-fngen-boardlarge.jpg)

Here's the pin diagram, with hex-labeled DAC pins (large-component connections
are marked with `#`). Component names are in square brackets.

```
             +---+                           +---+
             |   |                           |   |
       1   2 | 3 | 4   5   6   7   8   9   10| 11| 12  13  14
             |   |                           |   |
    +---------   ---------+         +---------   ---------+
a   |  #   .   #   .   #  |.   .   .|  #   5V  #   .   #  |.
    |        [P1]         |         |        [P2]         |
b   |  .   .   .   .   .  |.   .   .| 8B   9B  .   .   .  |.
    |                     |         |                     |
c   |  .   .   .   .   .  |.   .   .| AB   BB  .   .   .  |.
    +---------------------+         +---------------------+
d    +------------+  +------------+.  8A   9A  .   +-----------+
     |   [TR1]    |  |   [TR2]    |                |           |
e    | #   #   #  |. | #   #   #  |.  AA   BA  .   |   .   #   |
     +------------+  +------------+                |   [CHB]   |
f    +------------+  +------------+.  7A   6A  .   |   .   .   |
     |   [TR3]    |  |   [TR4]    |                |           |
g    | #   #   #  |. | #   #   #  |.  5A   4A  .   |   .   #   |
     +------------+  +------------+                |           |
h    +------------+  +------------+.  7B   6B  .   +-----------+
     |   [TR5]    |  |   [TR6]    |
i    | #   #   #  |. | #   #   #  |.  5B   4B  #---#---#---#
     +------------+  +------------+            |           |
j    +------------+  +------------+.   .   .   .   .   .   .
     |   [TR7]    |  |   [TR8]    |            |   [U1]   <
k    | #   #   #  |  | #   #   #  |.   .   .   .   .   .   .
     +------------+  +------------+            |           |
l    +------------+  +------------+.  3A   2A  #---#---#---#
     |   [TR9]    |  |   [TRA]    |
m    | #   #   #  |  | #   #   #  |.  1A   0A  .   +-----------+
     +------------+  +------------+                |           |
n    +------------+. +------------+.  3B   2B  .   |   .   #   |
     |   [TRB]    |  |   [TRC]    |                |   [CHA]   |
o    | #   #   #  |. | #   #   #  |.  1B   0B  .   |   .   .   |
     +------------+  +------------+                |           |
p      .   .   .   .   .   .   .   .   .   .   .   |   .   #   |
    +---------------------+         +--------------|------+    |
q   |  .   .   .   .   .  |.   .   .|  .   .   .   +------|----+
    |                     |         |                     |
r   |  .   .   .   .   .  |.   .   .|  .  GND  .   .   .  |.
    |        [P3]         |         |        [P4]         |
s   |  #   .   #   .   #  |.   .   .|  #   .   #   .   #  |.
    +---------   ---------+         +---------   ---------+
t      A8  A9|A10|A11 A12 A13 A14 A15  .   . | . | .   .   .
             |   |                           |   |
             |   |                           |   |
             +---+                           +---+
```

Connection map:

```
       1   2   3   4   5   6   7   8   9   10  11  12  13  14


a      5V  .  A8   .  GND  .   .   .  5V   5V  A9  .  GND  .
             [P1]                            [P2]
b      .   .   .   .   .   .   .   .  8B   9B  .   .   .   .

c      .   .   .   .   .   .   .   .  AB   BB  .   .   .   .

d      .   .   .   .   .   .   .   .  8A   9A  .   .   .   .
         [TR1]           [TR2]
e      #  RBA RAA  .   #  RBB RAB  .  AA   BA  .   .   .  OPB
                                                       [CHB]
f      .   .   .   .   .   .   .   .  7A   6A  .   .   .   .
         [TR3]           [TR4]
g      #  RAA R9A  .   #  RAB R9B  .  5A   4A  .   .   .  GND

h      .   .   .   .   .   .   .   .  7B   6B  .   .   .   .
         [TR5]           [TR6]
i      #  R9A R8A  .   #  R9B R8B  .  5B   4B GND  BB OPB OPB

j      .   .   .   .   .   .   .   .   .   .   .   .   .   .
         [TR7]           [TR8]                     [U1]
k      #  R8A R7A  .   #  R8B R7B  .   .   .   .   .   .   .

l      .   .   .   .   .   .   .   .  3A   2A  BA OPA OPA 5V
         [TR9]           [TRA]
m      #  R7A R6A  .   #  R7B R6B  .  1A   0A  .   .   .   .

n      .   .   .   .   .   .   .   .  3B   2B  .   .   .  OPA
         [TRB]           [TRC]                         [CHA]
o      #  R6A R5A  .   #  R6B R5B  .  1B   0B  .   .   .   .

p      .   .   .   .   .   .   .   .   .   .   .   .   .  GND

q      .   .   .   .   .   .   .   .   .   .   .   .   .   .

r      .   .   .   .   .   .   .   .   .  GND  .   .   .   .
             [P3]                            [P4]
s     GND  .  A10  .  5V   .   .   .  GND  .  A11  .  5V   .

t      A8  A9 A10 A11  .   .   .   .   .   .   .   .   .   .
```
