#include "9cc.h"

Node *code[100];

Node *stmt(Token *token);
Node *expr(Token *token);
Node *assign(Token *token);
Node *equality(Token *token);
Node *relational(Token *token);
Node *add(Token *token);
Node *mul(Token *token);
Node *unary(Token *token);
Node *primary(Token *token);

bool consume(Token *token, char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  *token = *(token->next);
  return true;
}

bool at_eof(Token *token) {
  return token->kind == TK_EOF;
}

void expect(Token *token, char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "the next token is not %c", op);
  *token = *(token->next);
}

int expect_number(Token *token) {
  if (token->kind != TK_NUM)
    error_at(token->str, "not integer");
  int val = token->val;
  *token = *(token->next);
  return val;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// program = stmt*
void *program(Token *token) {
  int i = 0;
  while (!at_eof(token))
    code[i++] = stmt(token);
  code[i] = NULL;
}

// stmt = expr ";"
Node *stmt(Token *token) {
  Node *node = expr(token);
  expect(token, ";");
  return node;
}

// expr = assign
Node *expr(Token *token) {
  Node *node = assign(token);
}

// assign = equality ("=" assign)?
Node *assign(Token *token) {
  Node *node = equality(token);
  if (consume(token, "="))
    node = new_binary(ND_ASSIGN, node, assign(token));
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(Token *token) {
  Node *node = relational(token);

  for(;;) {
    if (consume(token, "=="))
      node = new_binary(ND_EQ, node, relational(token));
    else if (consume(token, "!="))
      node = new_binary(ND_NE, node, relational(token));
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(Token *token) {
  Node *node = add(token);

  for(;;) {
    if (consume(token, "<"))
      node = new_binary(ND_LT, node, add(token));
    else if (consume(token, "<="))
      node = new_binary(ND_LE, node, add(token));
    else if (consume(token, ">"))
      node = new_binary(ND_LT, add(token), node);
    else if (consume(token, ">="))
      node = new_binary(ND_LE, add(token), node);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
Node *add(Token *token) {
  Node *node = mul(token);

  for(;;) {
    if (consume(token, "+"))
      node = new_binary(ND_ADD, node, mul(token));
    else if (consume(token, "-"))
      node = new_binary(ND_SUB, node, mul(token));
    else
      return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(Token *token) {
  Node *node = unary(token);

  for(;;) {
    if (consume(token, "*"))
      node = new_binary(ND_MUL, node, unary(token));
    else if (consume(token, "/"))
      node = new_binary(ND_DIV, node, unary(token));
    else
      return node;
  }
}

// unary = ("+" | "-")? primary
Node *unary(Token *token) {
  if (consume(token, "+"))
    return unary(token);
  if (consume(token, "-"))
    return new_binary(ND_SUB, new_num(0), unary(token));
  return primary(token);
}

// primary = num | ident | "(" expr ")"
Node *primary(Token *token) {
  if (consume(token, "(")) {
    Node *node = expr(token);
    expect(token, ")");
    return node;
  }

  return new_num(expect_number(token));
}
