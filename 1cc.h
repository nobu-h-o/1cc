#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// tokenize.c

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
  int len;
};

Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);

// parse.c

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NEG,
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_NUM,
} NodeKind;

// AST node type
typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;  // left hand side
  Node *rhs;  // right hand side
  int val;
};

Node *parse(Token *tok);

// codegen.c

void codegen(Node *node);
