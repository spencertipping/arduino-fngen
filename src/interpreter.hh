#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "emit.hh"
#include "repeat.hh"
#include "wavetables.hh"

#define STACK_SIZE   4
#define PROGRAM_SIZE 32

char const *const commands = " +*-^vqs:./abcdefO01234567";
uint8_t const command_len  = strlen(commands);

typedef uint16_t stack_value;

struct stack
{
  stack_value values[STACK_SIZE];
  uint8_t     top;

  stack_value peek() { return top > 0 ? values[top - 1] : 0; }
  stack_value pop()  { return top > 0 ? values[--top]   : 0; }
  void        push(stack_value const v)
    { if (top < STACK_SIZE) values[top++] = v; }

  void clear()
  {
    top = 0;
  }

  void init1(stack_value const v)
  {
    clear();
    push(v);
  }
};

struct program
{
  char code[PROGRAM_SIZE];

  void set(char const *const s)
  {
    for (int i = 0; i < PROGRAM_SIZE; ++i) code[i] = 0;
    for (int i = 0; s[i]; ++i)
    {
      char const *const k = strchr(commands, s[i]);
      code[i] = k >= commands && k < commands + command_len ? k - commands : 0;
    }
  }
};

void interpret(struct program const *const p, struct stack *const s)
{
  char c;

  stack_value v1;
  stack_value v2;

  for (int i = 0; i < PROGRAM_SIZE; ++i)
  {
    repeat_16( emit(); )
    switch (c = commands[p->code[i]])
    {
      case 'O': s->push(0); break;
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
        s->push(s->pop() >> 3 | c - '0' << 11);
        break;

      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        s->push(pots[c - 'a'].val() << 2);
        break;

      case '^': s->push(s->pop() << 4);                                 break;
      case 'v': s->push(s->pop() >> 4);                                 break;

      case '+': s->push(s->pop() + s->pop());                           break;
      case '-': s->push(-s->pop());                                     break;
      case '*': s->push((int32_t) s->pop() * (int32_t) s->pop() >> 12); break;
      case 'q': s->push(-(s->pop() > s->pop()));                        break;
      case 's': s->push(sine((uint32_t) s->pop() * 16) >> 4);           break;
      case ':': s->push(s->peek());                                     break;
      case '.': s->pop();                                               break;
      case '/': v1 = s->pop(); v2 = s->pop(); s->push(v1); s->push(v2); break;
    }
  }
}

#endif
