#include "1cc.h"

// Local variables
LVar *locals;

char *strndup(const char *s, size_t n) {
  char *p = calloc(n + 1, sizeof(char));
  memcpy(p, s, n);
  return p;
}

static Node *expr(Token **rest, Token *tok);
static Node *expr_stmt(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

// Find variable by name. Return NULL if not found.
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->loc, var->name, var->len))
      return var;
  return NULL;
}

// Consume identifier token
Token *consume_ident(Token **rest, Token *tok) {
  if (tok->kind == TK_IDENT) {
    *rest = tok->next;
    return tok;
  }
  *rest = tok;
  return NULL;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node* lhs, Node* rhs){
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_unary(NodeKind kind, Node* expr){
  Node *node = new_node(kind);
  node->lhs = expr;
  return node;
}

Node *new_num(int val){
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}


Node *stmt(Token **rest, Token *tok){
  if (tok->kind == TK_RETURN) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(&tok, tok->next);
    *rest = skip(tok, ";");
    return node;
  }

  if (equal(tok, ";")) {
    Node *node = new_node(ND_EMPTY_STMT);
    *rest = tok->next;
    return node;
  }

  if (tok->kind == TK_IF) {
    Node *node = new_node(ND_IF);
    tok = skip(tok->next, "(");
    node->cond = expr(&tok, tok);
    tok = skip(tok, ")");
    node->then = stmt(&tok, tok);
    if (tok->kind == TK_ELSE) {
      node->els = stmt(&tok, tok->next);
    }
    *rest = tok;
    return node;
  }

  if (tok->kind == TK_WHILE) {
    Node *node = new_node(ND_WHILE);
    tok = skip(tok->next, "(");
    node->cond = expr(&tok, tok);
    tok = skip(tok, ")");
    node->body = stmt(&tok, tok);
    *rest = tok;
    return node;
  }

  if (tok->kind == TK_FOR) {
    Node *node = new_node(ND_FOR);
    tok = skip(tok->next, "(");
    
    if (!equal(tok, ";")) {
      node->init = expr(&tok, tok);
    }
    tok = skip(tok, ";");
    
    if (!equal(tok, ";")) {
      node->cond = expr(&tok, tok);
    }
    tok = skip(tok, ";");
    
    if (!equal(tok, ")")) {
      node->inc = expr(&tok, tok);
    }
    tok = skip(tok, ")");
    
    node->body = stmt(&tok, tok);
    *rest = tok;
    return node;
  }
  
  return expr_stmt(rest, tok);
}

Node *expr_stmt(Token **rest, Token *tok){
  Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
  *rest = skip(tok, ";");
  return node;
}

Node *expr(Token **rest, Token *tok) {
  return assign(rest, tok);
}

Node *assign(Token **rest, Token *tok){
  Node *node = equality(&tok, tok);
  if(equal(tok, "=")) node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
  *rest = tok;
  return node;
}

Node *equality(Token **rest, Token *tok){
  Node *node = relational(&tok, tok);
  for(;;){
    if(equal(tok, "==")){
      node = new_binary(ND_EQ, node, relational(&tok, tok->next));
      continue;
    }
    if(equal(tok, "!=")){
      node = new_binary(ND_NE, node, relational(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

Node *relational(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);

  for (;;) {
    if (equal(tok, "<")) {
      node = new_binary(ND_LT, node, add(&tok, tok->next));
      continue;
    }

    if (equal(tok, "<=")) {
      node = new_binary(ND_LE, node, add(&tok, tok->next));
      continue;
    }

    if (equal(tok, ">")) {
      node = new_binary(ND_LT, add(&tok, tok->next), node);
      continue;
    }

    if (equal(tok, ">=")) {
      node = new_binary(ND_LE, add(&tok, tok->next), node);
      continue;
    }

    *rest = tok;
    return node;
  }
}

Node *add(Token **rest, Token *tok) {
  Node *node = mul(&tok, tok);

  for(;;){
    if(equal(tok, "+")){
      node = new_binary(ND_ADD, node, mul(&tok, tok->next));
      continue;
    }
    if(equal(tok, "-")){
      node = new_binary(ND_SUB, node, mul(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

Node *mul(Token **rest, Token *tok) {
  Node *node = unary(&tok, tok);

  for (;;) {
    if (equal(tok, "*")) {
      node = new_binary(ND_MUL, node, unary(&tok, tok->next));
      continue;
    }

    if (equal(tok, "/")) {
      node = new_binary(ND_DIV, node, unary(&tok, tok->next));
      continue;
    }

    *rest = tok;
    return node;
  }
}

Node *unary(Token **rest, Token *tok) {
  if (equal(tok, "+"))
    return unary(rest, tok->next);

  if (equal(tok, "-"))
    return new_unary(ND_NEG, unary(rest, tok->next));

  return primary(rest, tok);
}

Node *primary(Token **rest, Token *tok) {
  if (equal(tok, "(")) {
    Node *node = expr(&tok, tok->next);
    *rest = skip(tok, ")");
    return node;
  }

  if (tok->kind == TK_NUM) {
    Node *node = new_num(tok->val);
    *rest = tok->next;
    return node;
  }

  Token *ident_tok = consume_ident(&tok, tok);
  if (ident_tok) {
    if (equal(tok, "(")) {
      Node *node = calloc(1, sizeof(Node));
      node->kind = ND_FUNCALL;
      node->funcname = strndup(ident_tok->loc, ident_tok->len);
      
      tok = tok->next;
      Node head = {};
      Node *cur = &head;
      
      while (!equal(tok, ")")) {
        if (cur != &head) tok = skip(tok, ",");
        cur = cur->next = expr(&tok, tok);
      }
      
      node->args = head.next;
      *rest = skip(tok, ")");
      return node;
    }
    
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(ident_tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = ident_tok->loc;
      lvar->len = ident_tok->len;
      if (locals)
        lvar->offset = locals->offset + 8;
      else
        lvar->offset = 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    *rest = tok;
    return node;
  }

  error_tok(tok, "expected an expression");
}

Node *function(Token **rest, Token *tok) {
  Token *name_tok = consume_ident(&tok, tok);
  if (!name_tok) error_tok(tok, "expected function name");
  
  Node *fn = calloc(1, sizeof(Node));
  fn->kind = ND_FUNCTION;
  fn->funcname = strndup(name_tok->loc, name_tok->len);
  
  locals = NULL;
  
  tok = skip(tok, "(");
  Node param_head = {};
  Node *param_cur = &param_head;
  
  while (!equal(tok, ")")) {
    if (param_cur != &param_head) tok = skip(tok, ",");
    Token *param_tok = consume_ident(&tok, tok);
    if (!param_tok) error_tok(tok, "expected parameter name");
    
    Node *param = calloc(1, sizeof(Node));
    param->kind = ND_LVAR;
    
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = param_tok->loc;
    lvar->len = param_tok->len;
    if (locals)
      lvar->offset = locals->offset + 8;
    else
      lvar->offset = 8;
    param->offset = lvar->offset;
    locals = lvar;
    
    param_cur = param_cur->next = param;
  }
  
  fn->params = param_head.next;
  tok = skip(tok, ")");
  tok = skip(tok, "{");
  
  Node stmt_head = {};
  Node *stmt_cur = &stmt_head;
  while (!equal(tok, "}")) {
    stmt_cur = stmt_cur->next = stmt(&tok, tok);
  }
  
  fn->body = stmt_head.next;
  fn->locals = locals;
  *rest = skip(tok, "}");
  return fn;
}

Node *parse(Token *tok) {
  Node head = {};
  Node *cur = &head;
  
  while(tok->kind != TK_EOF) {
    if (tok->kind == TK_IDENT) {
      Token *next = tok->next;
      if (next && equal(next, "(")) {
        cur = cur->next = function(&tok, tok);
      } else {
        error_tok(tok, "expected function declaration");
      }
    } else {
      error_tok(tok, "expected function declaration");
    }
  }
  return head.next;
}
