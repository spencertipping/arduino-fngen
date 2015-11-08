#ifndef REPEAT_H
#define REPEAT_H

#define repeat_2(x) x x
#define repeat_4(x) repeat_2(repeat_2(x))
#define repeat_8(x) repeat_4(repeat_2(x))
#define repeat_16(x) repeat_4(repeat_4(x))
#define repeat_32(x) repeat_8(repeat_4(x))
#define repeat_64(x) repeat_8(repeat_8(x))

#endif
