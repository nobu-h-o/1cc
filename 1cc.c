#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Types of tokens
typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

// Token types
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op) {
  if(token -> kind != TK_RESERVED || token -> str[0] != op) {
    return false;
  }
  token = token -> next;
  return true;
}

void expect(char op) {
  if(token -> kind != TK_RESERVED || token -> str[0] != op) {
    error("not '%c'", op);
  }
  token = token -> next;
}

int expect_number() {
  if(token -> kind != TK_NUM) {
    error("Not a number");
  }
  int val = token -> val;
  token = token -> next;
  return val;
}

bool at_eof(){
  return token -> kind == TK_EOF;
}

// Create a new Token and append to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok -> kind = kind;
  tok -> str = str;
  cur -> next = tok;
  return tok;
}

// Tokenize and return p
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  
  while(*p){
    if(isspace(*p)){
      p++;
      continue;
    }

    if(*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p);
      cur -> val = strtol(p, &p, 10);
      continue;
    }

    error("Could not tokenize");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "The number of arguments is incorrect\n");
    return 1;
  }

  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number());

  while(!at_eof()) {
    if(consume('+')){
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}
