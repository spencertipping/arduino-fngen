#ifndef EMIT_H
#define EMIT_H

#define BUF_SIZE 2048
uint8_t a_buf[BUF_SIZE];
uint8_t c_buf[BUF_SIZE];
uint8_t l_buf[BUF_SIZE];

uint16_t emit_mask = 0xffff;

//*
// Original (less expensive) version:
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
// */

/*
// More expensive version to test:
#define emit()                                          \
  do {                                                  \
    uint16_t EMIT_t = TCNT3 & emit_mask;                \
    uint8_t  EMIT_ah = a_buf[EMIT_t] & 0xf0 >> 4;       \
    uint8_t  EMIT_al = a_buf[EMIT_t] & 0x0f;            \
    uint8_t  EMIT_ch = c_buf[EMIT_t] & 0xf0 >> 4;       \
    uint8_t  EMIT_cl = c_buf[EMIT_t] & 0x0f;            \
    uint8_t  EMIT_lh = l_buf[EMIT_t] & 0xf0 >> 4;       \
    uint8_t  EMIT_ll = l_buf[EMIT_t] & 0x0f;            \
    uint8_t  EMIT_a = EMIT_ah << 4 | EMIT_al;           \
    uint8_t  EMIT_c = EMIT_ch << 4 | EMIT_cl;           \
    uint8_t  EMIT_l = EMIT_lh << 4 | EMIT_ll;           \
    PORTA = EMIT_a;                                     \
    PORTC = EMIT_c;                                     \
    PORTL = EMIT_l;                                     \
  } while (0)
// */

#endif
