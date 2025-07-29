#include "1cc.h"

static int depth;
static int label_count = 0;

static void push(void){
  printf("  push %%rax\n");
  depth++;
}
static void pop(char *arg) {
  printf("  pop %s\n", arg);
  depth--;
}

static void gen_addr(Node* node){
  if(node->kind == ND_LVAR){
    printf("  lea %d(%%rbp), %%rax\n", -node->offset);
    return;
  }
  error("not an lvalue");
}

static void gen_expr(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  mov $%d, %%rax\n", node->val);
    return;
  case ND_NEG:
    gen_expr(node->lhs);
    printf("  neg %%rax\n");
    return;
  case ND_LVAR:
    gen_addr(node);
    printf("  mov (%%rax), %%rax\n");
    return;
  case ND_ASSIGN:
    gen_addr(node->lhs);
    push();
    gen_expr(node->rhs);
    pop("%rdi");
    printf("  mov %%rax, (%%rdi)\n");
    return;
  }
  gen_expr(node->rhs);
  push();
  gen_expr(node->lhs);
  pop("%rdi");

  switch (node->kind) {
  case ND_ADD:
    printf("  add %%rdi, %%rax\n");
    return;
  case ND_SUB:
    printf("  sub %%rdi, %%rax\n");
    return;
  case ND_MUL:
    printf("  imul %%rdi, %%rax\n");
    return;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv %%rdi\n");
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    printf("  cmp %%rdi, %%rax\n");

    if (node->kind == ND_EQ)
      printf("  sete %%al\n");
    else if (node->kind == ND_NE)
      printf("  setne %%al\n");
    else if (node->kind == ND_LT)
      printf("  setl %%al\n");
    else if (node->kind == ND_LE)
      printf("  setle %%al\n");

    printf("  movzb %%al, %%rax\n");
    return;
  }

  error("invalid expression");
}

void gen_stmt(Node* node){
  if(node->kind == ND_EXPR_STMT){
    gen_expr(node->lhs);
    return;
  }

  if (node->kind == ND_EMPTY_STMT) {
    return;
  }

  if (node->kind == ND_RETURN) {
    gen_expr(node->lhs);
    printf("  mov %%rbp, %%rsp\n");
    printf("  pop %%rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->kind == ND_IF) {
    int label_num = label_count++;
    gen_expr(node->cond);
    printf("  cmp $0, %%rax\n");
    if (node->els) {
      printf("  je .Lelse%d\n", label_num);
      gen_stmt(node->then);
      printf("  jmp .Lend%d\n", label_num);
      printf(".Lelse%d:\n", label_num);
      gen_stmt(node->els);
      printf(".Lend%d:\n", label_num);
    } else {
      printf("  je .Lend%d\n", label_num);
      gen_stmt(node->then);
      printf(".Lend%d:\n", label_num);
    }
    return;
  }

  if (node->kind == ND_WHILE) {
    int label_num = label_count++;
    printf(".Lbegin%d:\n", label_num);
    gen_expr(node->cond);
    printf("  cmp $0, %%rax\n");
    printf("  je .Lend%d\n", label_num);
    gen_stmt(node->body);
    printf("  jmp .Lbegin%d\n", label_num);
    printf(".Lend%d:\n", label_num);
    return;
  }

  if (node->kind == ND_FOR) {
    int label_num = label_count++;
    if (node->init) {
      gen_expr(node->init);
    }
    printf(".Lbegin%d:\n", label_num);
    if (node->cond) {
      gen_expr(node->cond);
      printf("  cmp $0, %%rax\n");
      printf("  je .Lend%d\n", label_num);
    }
    gen_stmt(node->body);
    if (node->inc) {
      gen_expr(node->inc);
    }
    printf("  jmp .Lbegin%d\n", label_num);
    printf(".Lend%d:\n", label_num);
    return;
  }

  error("invalid statement");
}

void codegen(Node *node) {
  printf("  .globl main\n");
  printf("main:\n");

  printf("  push %%rbp\n");
  printf("  mov %%rsp, %%rbp\n");
  
  // Calculate stack size based on local variables
  int stack_size = 0;
  for (LVar *lvar = locals; lvar; lvar = lvar->next) {
    if (lvar->offset > stack_size)
      stack_size = lvar->offset;
  }
  // Align to 16 bytes
  stack_size = (stack_size + 15) / 16 * 16;
  
  if (stack_size > 0)
    printf("  sub $%d, %%rsp\n", stack_size);

  for(Node *n = node; n; n = n->next){
    gen_stmt(n);
    assert(depth==0);
  }
  printf("  mov %%rbp, %%rsp\n");
  printf("  pop %%rbp\n");
  printf("  ret\n");
}
