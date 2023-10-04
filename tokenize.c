#include "9cc.h"

char *user_input;

bool startwith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// reports an error and exit
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

Token *tokenize(char *program) {
  user_input = program;
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // skip white spaces
    if (isspace(*p)) {
      p++;
      continue;
    }

    // multi-letter punctuator
    if (startwith(p, "==") || startwith(p, "!=") ||
	startwith(p, "<=") || startwith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // single-letter punctuator
    if (strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p++, 1);
      cur->len = 1;
      continue;
    }

    error_at(cur->str, "invalid token");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

