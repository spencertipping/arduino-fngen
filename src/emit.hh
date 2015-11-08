#ifndef EMIT_H
#define EMIT_H

#define BUF_SIZE 2048
uint8_t a_buf[BUF_SIZE];
uint8_t c_buf[BUF_SIZE];
uint8_t l_buf[BUF_SIZE];

uint16_t emit_mask = 0xffff;

#define emit()                                          \
  do {                                                  \
    uint16_t EMIT_t = TCNT3 & emit_mask;                \
    uint8_t  EMIT_a = a_buf[EMIT_t];                    \
    uint8_t  EMIT_c = c_buf[EMIT_t];                    \
    uint8_t  EMIT_l = l_buf[EMIT_t];                    \
    PORTA = EMIT_a;                                     \
    PORTC = EMIT_c;                                     \
    PORTL = EMIT_l;                                     \
  } while (0)

#endif
