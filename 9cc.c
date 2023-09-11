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

typedef struct Token Token;

// token type
struct Token {
  TokenKind kind; // type of token
  Token *next;    // next input token
  int val;        // value of TK_NUM
  char *str;      // token literal
};

// current token
Token *token;

// reports an error and exit
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// consumes the current token if it matches 'op'
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'the next token is not %c", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error("not integer");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // skip white spaces
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("invalid token");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
    return 1;
  }

  // tokenize
  token = tokenize(argv[1]);
  
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // the first token must be a number
  printf("  mov rax, %d\n", expect_number());

  // output assembly
  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }
  
  printf("  ret\n");
  return 0;
}
