# A quick aside about compilation
Arduino programs don't technically need to be precompiled, but they have a
weird directory structure that I didn't want to mess with. So this project is
written like a normal C++ program and assembled into an Arduino source file by
the [build script](../build).

Things that happen during the build process:

1. Any `#include ""` (include with quote-marks) are inlined.
2. Any `#perl ...` directives are evaluated by perl and the results placed into
   the source code. This is how I generate things like sine wave tables.

The result of all of this goes into `../arduinofngen.ino`, which can be
uploaded directly by the Arduino IDE.
