#include "1cc.h"

static char *current_input;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void verror_at(char *loc, char *fmt, va_list ap){
  int pos = loc - current_input;
  fprintf(stderr, "%s\n", current_input);
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(loc, fmt, ap);
}

void error_tok(Token *tok, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->loc, fmt, ap);
}

bool equal(Token *tok, char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

Token *skip(Token *tok, char *op) {
  if (!equal(tok, op)) error_tok(tok, "expected '%s'", op);
  return tok->next;
}

// Create a new Token and append to cur
Token *new_token(TokenKind kind, char *start, char *end) {
  Token *tok = calloc(1, sizeof(Token));
  tok -> kind = kind;
  tok -> loc = start;
  tok -> len = end - start;
  return tok;
}

bool startswith(char *p, char *q){
  return memcmp(p, q, strlen(q))==0;
}

int read_punct(char *p){
  if(startswith(p, "==") || startswith(p, "!=") ||
    startswith(p, "<=") || startswith(p, ">="))
    return 2;
  if(ispunct(*p)) return 1;
  else return 0;
}

// Tokenize and return p
Token *tokenize(char *p) {
  current_input = p;
  Token head;
  head.next = NULL;
  Token *cur = &head;
  
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (isdigit(*p)) {
      cur = cur->next = new_token(TK_NUM, p, p);
      char *q = p;
      cur->val = strtoul(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    if('a'<=*p && *p<='z'){
      cur = cur->next = new_token(TK_IDENT, p, p+1);
      p++;
      continue;
    }

    int punct_len = read_punct(p);
    if (punct_len) {
      cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
      p += cur->len;
      continue;
    }

    error_at(p, "invalid token");
  }

  cur = cur->next = new_token(TK_EOF, p, p);
  return head.next;
}
