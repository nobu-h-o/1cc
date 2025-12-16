# 1cc - Hand-Written C Compiler

A minimal C compiler built entirely from scratch using only C. 1cc is the foundation of a compiler learning series, implementing every component by hand to understand the fundamentals of compilation.

## Compiler Evolution Series

1cc is the first step in a compiler learning journey:

### **1cc** → 2cc → 3cc

| Stage | **1cc** | 2cc | 3cc |
|-------|---------|-----|-----|
| **Lexer** | **Hand-written tokenizer** ⭐ | Flex | Flex |
| **Parser** | **Recursive descent** ⭐ | Bison | Bison |
| **Language** | **C** | C | C++ |
| **Code Gen** | **x86-64 Assembly** ⭐ | ARM64 Assembly | LLVM IR |
| **Target** | **x86-64 only** | ARM64 only | Multi-arch |
| **Build** | **Make** | Make | CMake |

## Philosophy: Learning by Building

1cc takes the **purist approach** to learning compilers:

### No Libraries, No Generators, Just Code

Unlike 2cc and 3cc which use tools like Flex and Bison, 1cc implements everything manually:

- **Tokenizer**: Hand-written character-by-character scanning
- **Parser**: Hand-written recursive descent parser
- **Code Generator**: Direct x86-64 assembly output
- **No Dependencies**: Just C and a C compiler

### Why Start Here?

**Understanding beats convenience.** While tools like Flex and Bison are powerful, building a compiler from scratch teaches you:

1. **How tokenization really works** - Pattern matching, lookahead, state management
2. **How parsing really works** - Grammar rules, precedence, associativity
3. **How code generation works** - Register allocation, stack management, calling conventions
4. **How everything fits together** - The complete pipeline from source to assembly

Once you understand these fundamentals, tools like Flex/Bison/LLVM make sense. Without this foundation, they're just magic.

## What You'll Learn

### 1. Tokenization (tokenize.c)

How to convert source code into tokens:

```c
// Input: "x = 5 + 10"
// Process:
while (*p) {
    if (isspace(*p)) { p++; continue; }  // Skip whitespace

    if (isdigit(*p)) {
        // Parse number: "5" → TK_NUM(5)
        cur->val = strtoul(p, &p, 10);
    }

    if (isalpha(*p)) {
        // Parse identifier: "x" → TK_IDENT("x")
        while (isalnum(*p)) p++;
    }

    if (*p == '+') {
        // Parse operator: "+" → TK_PUNCT("+")
        cur = new_token(TK_PUNCT, p, p + 1);
    }
}
// Output: [IDENT("x"), PUNCT("="), NUM(5), PUNCT("+"), NUM(10)]
```

**Key concepts:**
- Lookahead for multi-character operators (`==`, `<=`)
- Keyword detection (`return`, `if`, `while`)
- Error reporting with position tracking

### 2. Parsing (parse.c)

How to build an Abstract Syntax Tree using recursive descent:

```c
// Grammar:
// expr    = equality
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | ">" add | "<=" add | ">=" add)*
// add     = mul ("+" mul | "-" mul)*
// mul     = unary ("*" unary | "/" unary)*
// unary   = ("+" | "-")? primary
// primary = num | ident | "(" expr ")"

Node *add(Token **rest, Token *tok) {
    Node *node = mul(&tok, tok);

    while (equal(tok, "+") || equal(tok, "-")) {
        Token *op = tok;
        tok = tok->next;

        if (equal(op, "+"))
            node = new_binary(ND_ADD, node, mul(&tok, tok));
        else
            node = new_binary(ND_SUB, node, mul(&tok, tok));
    }

    *rest = tok;
    return node;
}
```

**Key concepts:**
- Operator precedence via recursive function structure
- Left-associativity via while loops
- AST construction during parsing

### 3. Code Generation (codegen.c)

How to generate x86-64 assembly:

```c
// Input: AST for "x = 5 + 3"
// Output: x86-64 assembly

static void gen_expr(Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  mov rax, %d\n", node->val);  // Load immediate
        return;

    case ND_ADD:
        gen_expr(node->rhs);    // Evaluate right side → rax
        printf("  push rax\n");  // Save it
        gen_expr(node->lhs);    // Evaluate left side → rax
        printf("  pop rdi\n");   // Restore right side → rdi
        printf("  add rax, rdi\n");  // Add them
        return;

    case ND_LVAR:
        gen_addr(node);          // Get variable address → rax
        printf("  mov rax, [rax]\n");  // Load value
        return;
    }
}
```

**Key concepts:**
- Stack-based evaluation
- Register allocation (using rax, rdi, rsi, etc.)
- Variable storage on the stack
- Function calling conventions (System V AMD64 ABI)

## Features

1cc supports a C-like language with:

- **Data Types**: 32-bit signed integers (only type!)
- **Variables**: Local variables with automatic allocation
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`, unary `-`, unary `+`
  - Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Control Flow**:
  - `if` and `if-else` statements
  - `while` loops
  - `for` loops
- **Functions**:
  - Function definitions with parameters (up to 6 parameters)
  - Function calls
  - Recursive functions
- **Return statements**: `return` keyword

## Requirements

- **C Compiler**: gcc or clang
- **GNU Assembler**: `as` (for assembling generated code)
- **GNU Linker**: `ld` or `gcc` (for linking)
- **Make**: Build automation

## Building

```bash
make
```

This compiles all C source files (`tokenize.c`, `parse.c`, `codegen.c`, `main.c`) into the `1cc` executable.

## Usage

### Basic Workflow

1. **Compile source to assembly:**
```bash
./1cc "main() { return 42; }" > program.s
```

2. **Assemble to object file:**
```bash
gcc -o program program.s
```

3. **Run:**
```bash
./program
echo $?  # Prints: 42
```

### Examples

**Arithmetic:**
```bash
./1cc "main() { return 3 * (2 + 5); }" > tmp.s
gcc -o tmp tmp.s
./tmp
echo $?  # Prints: 21
```

**Variables:**
```bash
./1cc "main() { a = 5; b = 10; return a + b; }" > tmp.s
gcc -o tmp tmp.s
./tmp
echo $?  # Prints: 15
```

**Loops:**
```bash
./1cc "main() { sum = 0; for (i = 1; i <= 10; i = i + 1) sum = sum + i; return sum; }" > tmp.s
gcc -o tmp tmp.s
./tmp
echo $?  # Prints: 55
```

**Recursive functions:**
```bash
./1cc "fact(n) { if (n <= 1) return 1; return n * fact(n-1); } main() { return fact(5); }" > tmp.s
gcc -o tmp tmp.s
./tmp
echo $?  # Prints: 120
```

**Fibonacci:**
```bash
./1cc "fib(n) { if (n <= 1) return n; return fib(n-1) + fib(n-2); } main() { return fib(7); }" > tmp.s
gcc -o tmp tmp.s
./tmp
echo $?  # Prints: 13
```

## Generated Assembly

1cc generates Intel syntax x86-64 assembly. Here's what it looks like:

**Input:**
```c
main() { a = 5; return a; }
```

**Output:**
```asm
.intel_syntax noprefix
  .globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 16          # Allocate stack space for locals

  lea rax, [rbp-8]     # Get address of variable 'a'
  push rax
  mov rax, 5           # Load value 5
  pop rdi
  mov [rdi], rax       # Store: a = 5

  lea rax, [rbp-8]     # Get address of 'a'
  mov rax, [rax]       # Load value of 'a'

  mov rsp, rbp
  pop rbp
  ret
```

**Understanding the stack layout:**
```
High addresses
+------------------+
| Return address   |
+------------------+
| Saved RBP        | ← RBP points here
+------------------+
| Local var 'a'    | ← RBP-8
+------------------+
| Local var 'b'    | ← RBP-16
+------------------+
| ...              |
+------------------+ ← RSP
Low addresses
```

## Language Syntax

### Variables

No explicit declarations needed - variables are created on first assignment:

```c
main() {
    x = 5;
    y = 10;
    z = x + y;
    return z;  // Returns 15
}
```

### Control Flow

**If statements:**
```c
main() {
    a = 5;
    if (a > 3)
        return 1;
    else
        return 0;
}
```

**While loops:**
```c
main() {
    i = 0;
    sum = 0;
    while (i < 10) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;  // Returns 45
}
```

**For loops:**
```c
main() {
    sum = 0;
    for (i = 1; i <= 10; i = i + 1) {
        sum = sum + i;
    }
    return sum;  // Returns 55
}
```

### Functions

**Basic functions:**
```c
add(x, y) {
    return x + y;
}

main() {
    return add(3, 4);  // Returns 7
}
```

**Recursive functions:**
```c
sum(n) {
    if (n <= 0) return 0;
    return n + sum(n-1);
}

main() {
    return sum(10);  // Returns 55
}
```

## Running Tests

1cc includes 81 comprehensive test cases:

```bash
make test
# or
./test.sh
```

**Test categories:**
- Arithmetic expressions (15 tests)
- Comparison operators (12 tests)
- Variables (10 tests)
- Return statements (4 tests)
- If/else statements (8 tests)
- Loops (15 tests)
- Functions (17 tests)

All tests pass by checking the exit code of the compiled program.

## Project Structure

```
1cc/
├── 1cc.h           # Header with type definitions
├── tokenize.c      # Hand-written lexer
├── parse.c         # Recursive descent parser
├── codegen.c       # x86-64 code generator
├── main.c          # Compiler driver
├── test.sh         # Test suite (81 tests)
├── Makefile        # Build configuration
└── README.md       # This file
```

## Implementation Deep Dive

### Token Types

```c
typedef enum {
    TK_NUM,      // Numbers: 42, 123
    TK_IDENT,    // Identifiers: x, foo, main
    TK_PUNCT,    // Punctuation: +, -, *, /, ==, !=
    TK_RETURN,   // Keywords: return
    TK_IF,       // Keywords: if
    TK_ELSE,     // Keywords: else
    TK_WHILE,    // Keywords: while
    TK_FOR,      // Keywords: for
    TK_EOF,      // End of file
} TokenKind;
```

### AST Node Types

```c
typedef enum {
    ND_ADD,      // +
    ND_SUB,      // -
    ND_MUL,      // *
    ND_DIV,      // /
    ND_NEG,      // Unary -
    ND_EQ,       // ==
    ND_NE,       // !=
    ND_LT,       // <
    ND_LE,       // <=
    ND_NUM,      // Integer literal
    ND_LVAR,     // Local variable
    ND_ASSIGN,   // =
    ND_RETURN,   // return
    ND_IF,       // if statement
    ND_WHILE,    // while loop
    ND_FOR,      // for loop
    ND_FUNCALL,  // Function call
} NodeKind;
```

## The Compiler Pipeline

```
Source Code: "x = 5 + 3"
    ↓
┌─────────────────┐
│   Tokenizer     │  tokenize.c
│  (tokenize)     │  Manual character scanning
└────────┬────────┘
         │ [IDENT("x"), PUNCT("="), NUM(5), PUNCT("+"), NUM(3)]
         ↓
┌─────────────────┐
│     Parser      │  parse.c
│    (parse)      │  Recursive descent
└────────┬────────┘
         │ AST:
         │   ASSIGN
         │   ├─ LVAR("x")
         │   └─ ADD
         │      ├─ NUM(5)
         │      └─ NUM(3)
         ↓
┌─────────────────┐
│  Code Generator │  codegen.c
│   (codegen)     │  Stack-based x86-64 assembly
└────────┬────────┘
         │
         ↓
    x86-64 Assembly
```

## Why Hand-Written Matters

### Educational Benefits

**1cc teaches you things that tools hide:**

- **Tokenization**: How do you distinguish `<` from `<=`? Lookahead!
- **Parsing**: Why does `2 + 3 * 4` equal 14, not 20? Precedence via recursion!
- **Code gen**: Where do variables live? Stack offsets from RBP!
- **Debugging**: When something breaks, you understand every line of code

### Trade-offs

| Aspect | 1cc (Hand-written) | 2cc/3cc (Generated) |
|--------|-------------------|---------------------|
| **Understanding** | Complete | Abstracted away |
| **Control** | Total | Limited |
| **Code Size** | Larger | Smaller |
| **Maintainability** | Harder | Easier |
| **Correctness** | Manual verification | Proven algorithms |
| **Performance** | Depends on you | Optimized generators |

### When to Use Each Approach

**Use hand-written (1cc approach) when:**
- Learning compiler fundamentals
- Building a minimal, specific-purpose compiler
- You need absolute control over generated code
- The language is very simple

**Use generators (2cc/3cc approach) when:**
- Building production compilers
- Language grammar is complex
- You need formal correctness guarantees
- Maintainability matters more than control

## Common Pitfalls and How 1cc Handles Them

### 1. Operator Precedence

**Problem:** `2 + 3 * 4` should be 14, not 20

**Solution:** Function call hierarchy mirrors precedence
```c
expr → equality → relational → add → mul → unary → primary
```

Lower precedence operators are higher in the call chain.

### 2. Left Associativity

**Problem:** `10 - 3 - 2` should be 5, not 9

**Solution:** While loops in parsing functions
```c
Node *add(...) {
    Node *node = mul(...);
    while (equal(tok, "+") || equal(tok, "-")) {
        // Left side is already parsed (node)
        // Parse right side and combine
    }
}
```

### 3. Variable Scope

**Problem:** Where do variables live?

**Solution:** Stack-based allocation with RBP-relative offsets
```c
// Each variable gets a unique offset from RBP
// First variable: [rbp-8]
// Second variable: [rbp-16]
// Third variable: [rbp-24]
```

### 4. Function Calls

**Problem:** Passing arguments and getting return values

**Solution:** System V AMD64 ABI
```c
// First 6 args: rdi, rsi, rdx, rcx, r8, r9
// Return value: rax
add(x, y) { return x + y; }
// Caller:  mov rdi, 3; mov rsi, 4; call add
// Callee returns result in rax
```

## What's Next: Moving to 2cc

After mastering 1cc, you're ready for 2cc, which introduces:

**Flex (Lexer Generator):**
```flex
"return"    { return RETURN; }
[0-9]+      { yylval.number = atoi(yytext); return NUMBER; }
```
Replaces 100+ lines of manual scanning with declarative patterns.

**Bison (Parser Generator):**
```yacc
expr: expr ADD term { $$ = ast_add($1, $3); }
    | term { $$ = $1; }
    ;
```
Replaces recursive descent with formal grammar.

## Learning Resources

### Compiler Theory
- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook) - The inspiration for this project
- [Crafting Interpreters](https://craftinginterpreters.com/) - Excellent compiler tutorial
- [Engineering a Compiler](https://www.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-088478-0) - Comprehensive textbook

### x86-64 Assembly
- [Intel 64 and IA-32 Architectures Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [System V AMD64 ABI](https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-1.0.pdf) - Calling conventions
- [x86-64 Assembly Tutorial](https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf)

### Related Projects
- [chibicc](https://github.com/rui314/chibicc) - Small C compiler by Rui Ueyama
- [8cc](https://github.com/rui314/8cc) - Another C compiler by Rui Ueyama
- [TCC](https://bellard.org/tcc/) - Tiny C Compiler by Fabrice Bellard

## License

This is an educational project for learning compiler construction.

## Acknowledgments

Inspired by [chibicc](https://github.com/rui314/chibicc) and the excellent tutorial [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook).
