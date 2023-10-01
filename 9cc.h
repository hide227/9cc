#ifndef _9CC_H_
#define _9CC_H_

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

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


//
// parse.c
//

// kind of AST node
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_NUM, // integer
} NodeKind;

// type of AST node
typedef struct Node Node;
struct Node {
  NodeKind kind; // type of node
  Node *lhs;     // left-hand side
  Node *rhs;     // right-hand side
  int val;       // use if kind is ND_NUM
};


//
// parse.c
//
Node *expr(Token *token);


//
// codegen.c
//

void gen(Node *node);

#endif
