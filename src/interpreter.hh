#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "wavetables.hh"

#define STACK_SIZE   8
#define PROGRAM_SIZE 32

char const *const commands = " +*/-s=.x>abcdefk0123456789ABCDEF";
uint8_t const command_len  = strlen(commands);

typedef uint32_t stack_value;

struct stack
{
  stack_value values[STACK_SIZE];
  uint8_t     top;

  stack_value peek()                    { return values[top]; }
  stack_value pop()                     { return values[top--]; }
  void        push(stack_value const v) { values[++top] = v; }

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
    for (int i = 0; s[i]; ++i) code[i] = strchr(commands, s[i]) - commands;
  }
};

void interpret(struct program const *const p, struct stack *const s)
{
  char c;

  stack_value v1;
  stack_value v2;

  for (int i = 0; i < PROGRAM_SIZE; ++i)
    switch (c = commands[p->code[i]])
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        s->push(s->pop() << 4 | c - '0');
        break;

      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        s->push(s->pop() << 4 | c - 'A' + 0x0a);
        break;

      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        s->push(pots[commands[c] - 'a'].val());
        break;

      case 'k': s->push(0);                     break;
      case '+': s->push(s->pop() + s->pop());   break;
      case '-': s->push(0xffff - s->pop());     break;
      case '*': s->push(s->pop() * s->pop());   break;
      case '/': s->push(s->pop() >> 10);        break;
      case 's': s->push(sine(s->pop()));        break;
      case '=': s->push(s->peek());             break;
      case '.': s->pop();                       break;
      case '>': s->push(s->pop() > s->pop());   break;
      case 'x':
        v1 = s->pop();
        v2 = s->pop();
        s->push(v1);
        s->push(v2);
        break;

      default: break;
    }
}

#endif
