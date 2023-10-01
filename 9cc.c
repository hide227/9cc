#include "9cc.h"

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
    return 1;
  }

  // tokenize and parse input code
  Token *tok = tokenize(argv[1]);
  Node *node = expr(tok);

  // output the first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // traverse the AST to emit assembly
  gen(node);

  // a result must be at the top of the stack, so pop it
  // to RAX to make it a program exit code
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
