#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strndup(const char *s, size_t n);


// tokenize.c

// Types of tokens
typedef enum {
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
  TK_IDENT,
  TK_PUNCT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

// Token types
struct Token {
  TokenKind kind;   // Token kind
  Token *next;      // Next Token
  int val;          // Value (kind==TK_NUM)
  char *loc;        // Token location
  int len;          // Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);

// parse.c

typedef struct LVar LVar;

// Local variable type
struct LVar {
  LVar *next; // Next variable or NULL
  char *name; // Variable name
  int len;    // Name length
  int offset; // Offset from RBP
};

// Local variables
extern LVar *locals;

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
  ND_ASSIGN,
  ND_RETURN,
  ND_EXPR_STMT,
  ND_EMPTY_STMT,
  ND_IF,
  ND_WHILE,
  ND_FOR,
  ND_FUNCALL,
  ND_FUNCTION,
  ND_LVAR,
  ND_NUM,
} NodeKind;

// AST node type
typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node* next;
  Node *lhs;  // left hand side
  Node *rhs;  // right hand side
  Node *cond; // condition for if/while/for
  Node *then; // then clause for if
  Node *els;  // else clause for if
  Node *init; // init clause for for
  Node *inc;  // increment clause for for
  Node *body; // body for while/for
  Node *args; // function arguments
  Node *params; // function parameters
  char *funcname; // function name
  LVar *locals; // function local variables
  int val;
  int offset; // Used for ND_LVAR
};

LVar *find_lvar(Token *tok);
Token *consume_ident(Token **rest, Token *tok);
Node *parse(Token *tok);

// codegen.c

void codegen(Node *node);
