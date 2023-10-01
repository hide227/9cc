#ifndef _9CC_H_
#define _9CC_H_

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// kind of token
typedef enum {
  TK_RESERVED, // keywords or punctuators
  TK_NUM,      // integer literals
  TK_EOF,      // EOF markers
} TokenKind;

// token type
typedef struct Token Token;
struct Token {
  TokenKind kind; // type of token
  Token *next;    // next input token
  int val;        // value of TK_NUM
  char *str;      // token literal
  int len;        // token length
};

void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);

#endif
