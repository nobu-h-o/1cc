#include "1cc.h"

static int depth;
static int label_count = 0;

static void push(void){
  printf("  push rax\n");
  depth++;
}
static void pop(char *arg) {
  printf("  pop %s\n", arg);
  depth--;
}

static void gen_addr(Node* node){
  if(node->kind == ND_LVAR){
    printf("  lea rax, [rbp%+d]\n", -node->offset);
    return;
  }
  error("not an lvalue");
}

static void gen_expr(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  mov rax, %d\n", node->val);
    return;
  case ND_NEG:
    gen_expr(node->lhs);
    printf("  neg rax\n");
    return;
  case ND_LVAR:
    gen_addr(node);
    printf("  mov rax, [rax]\n");
    return;
  case ND_ASSIGN:
    gen_addr(node->lhs);
    push();
    gen_expr(node->rhs);
    pop("rdi");
    printf("  mov [rdi], rax\n");
    return;
  case ND_FUNCALL: {
    int arg_count = 0;
    char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    
    for (Node *arg = node->args; arg; arg = arg->next) {
      gen_expr(arg);
      push();
      arg_count++;
    }
    
    for (int i = arg_count - 1; i >= 0; i--) {
      pop(regs[i]);
    }
    
    printf("  call %s\n", node->funcname);
    return;
  }
  }
  gen_expr(node->rhs);
  push();
  gen_expr(node->lhs);
  pop("rdi");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    return;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    return;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    return;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    printf("  cmp rax, rdi\n");

    if (node->kind == ND_EQ)
      printf("  sete al\n");
    else if (node->kind == ND_NE)
      printf("  setne al\n");
    else if (node->kind == ND_LT)
      printf("  setl al\n");
    else if (node->kind == ND_LE)
      printf("  setle al\n");

    printf("  movzx rax, al\n");
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
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->kind == ND_FUNCTION) {
    printf("  .globl %s\n", node->funcname);
    printf("%s:\n", node->funcname);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    
    int stack_size = 0;
    for (LVar *lvar = node->locals; lvar; lvar = lvar->next) {
      if (lvar->offset > stack_size)
        stack_size = lvar->offset;
    }
    
    // Ensure 16-byte stack alignment
    if (stack_size == 0) {
      stack_size = 16; // Minimum for alignment
    } else {
      stack_size = (stack_size + 15) / 16 * 16;
    }
    printf("  sub rsp, %d\n", stack_size);
    
    char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    int param_count = 0;
    for (Node *param = node->params; param; param = param->next) {
      printf("  mov [rbp%+d], %s\n", -param->offset, regs[param_count]);
      param_count++;
    }
    
    for (Node *stmt = node->body; stmt; stmt = stmt->next) {
      gen_stmt(stmt);
      assert(depth == 0);
    }
    return;
  }

  if (node->kind == ND_IF) {
    int label_num = label_count++;
    gen_expr(node->cond);
    printf("  cmp rax, 0\n");
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
    printf("  jmp .Lcond%d\n", label_num);
    printf(".Lbegin%d:\n", label_num);
    gen_stmt(node->body);
    printf(".Lcond%d:\n", label_num);
    gen_expr(node->cond);
    printf("  cmp rax, 0\n");
    printf("  jne .Lbegin%d\n", label_num);
    return;
  }

  if (node->kind == ND_FOR) {
    int label_num = label_count++;
    if (node->init) {
      gen_expr(node->init);
    }
    printf("  jmp .Lcond%d\n", label_num);
    printf(".Lbegin%d:\n", label_num);
    gen_stmt(node->body);
    if (node->inc) {
      gen_expr(node->inc);
    }
    printf(".Lcond%d:\n", label_num);
    if (node->cond) {
      gen_expr(node->cond);
      printf("  cmp rax, 0\n");
      printf("  jne .Lbegin%d\n", label_num);
    } else {
      printf("  jmp .Lcond%d\n", label_num);
    }
    return;
  }

  error("invalid statement");
}

void codegen(Node *node) {
  printf(".intel_syntax noprefix\n");
  
  for(Node *n = node; n; n = n->next){
    gen_stmt(n);
  }
}
