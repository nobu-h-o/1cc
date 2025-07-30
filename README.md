# 1cc
1cc is a toy compiler for C.

## Progress
- [x] Basic Math
- [x] Tokenizer
- [X] Lexical analysis
- [X] Parser
- [X] Variables
- [X] Functions
- [ ] Types

## Compiling and executing
Use the makefile to compile 1cc to bytecode.
```shell
make
```
Next, you can execute the bytecode and input its output to a temporary file. We'll use simple arithmetic as an example.
```shell
./1cc "main(){ a = -3*(3+2*-4); return a;}" > tmp.s
```
We can check the assembly code with the 'cat' command.
```shell
cat tmp.s
```
<details>
<summary>Here's an example:</summary>
  
```asm
.intel_syntax noprefix
  .globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  lea rax, [rbp-8]
  push rax
  mov rax, 4
  neg rax
  push rax
  mov rax, 2
  pop rdi
  imul rax, rdi
  push rax
  mov rax, 3
  pop rdi
  add rax, rdi
  push rax
  mov rax, 3
  neg rax
  pop rdi
  imul rax, rdi
  pop rdi
  mov [rdi], rax
  lea rax, [rbp-8]
  mov rax, [rax]
  mov rsp, rbp
  pop rbp
  ret
```
</details>

We then compile the assembly code to bytecode.
```shell
gcc -o tmp tmp.s
```
Then, we execute the bytecode.
```shell
./tmp
```
This shouldn't output anything. The result of the calculation would be available as a return value, which you can check with the following command.
```shell
echo $?
```
This should return 15.

## Unit Testing
You can also run the numerous unit tests available in the test.sh file. You can run it with ``./test.sh``, but it is also available with the following command.
```shell
make test
```
``make test`` can be used both before and after compiling 1cc, while ``./test.sh`` only works after compilation.

<details>
<summary>Here's an example:</summary>
  
```
sh ./test.sh
main() { return 0; } => 0
main() { return 42; } => 42
main() { return 3 * (2 + 3); } => 15
main() { return 8 / (1 + 1); } => 4
main() { return (7 + 3) * (9 - 8) / 1; } => 10
main() { return 10 * (1 + 2) / 3; } => 10
main() { return 1 + 2 * 3 + 4; } => 11
main() { return 2 * 3 + 2 * 4; } => 14
main() { return 24 / 4 + 4; } => 10
main() { return 1 + 10 / 2; } => 6
main() { return 3 * (2 + (1 + 4)); } => 21
main() { return (2 + 2) * (3 - 1); } => 8
main() { return (2 + 4) * (1 + 5); } => 36
main() { return ((((3)))); } => 3
main() { return 3 * (+3 - 1); } => 6
main() { return -3 + (2 * 5); } => 7
main() { return 10 / +2; } => 5
main() { return -3 * (-4 + -1); } => 15
main() { return   1 +  2+ 3 +4+5+   6 + 7   +8+9  +  10; } => 55
main() { return   (   5   +   5  )  *  (   5  +   5   ); } => 100
main() { return 0==1; } => 0
main() { return 42==42; } => 1
main() { return 0!=1; } => 1
main() { return 42!=42; } => 0
main() { return 0<1; } => 1
main() { return 1<1; } => 0
main() { return 2<1; } => 0
main() { return 0<=1; } => 1
main() { return 1<=1; } => 1
main() { return 2<=1; } => 0
main() { return 1>0; } => 1
main() { return 1>1; } => 0
main() { return 1>2; } => 0
main() { return 1>=0; } => 1
main() { return 1>=1; } => 1
main() { return 1>=2; } => 0
main() { a=3; return a; } => 3
main() { a=3; z=5; return a+z; } => 8
main() { a=b=3; return a+b; } => 6
main() { a=3*5; return a; } => 15
main() { a=4*8*5; b=16*2; return a/b; } => 5
main() { foo=1; bar=2+3; return foo+bar; } => 6
main() { abc=10; xyz=5; return abc-xyz; } => 5
main() { foo=3; foo=foo*2; return foo; } => 6
main() { hello=7; world=hello*2; return hello+world; } => 21
main() { variable1=25; variable2=variable1*4; return variable2; } => 100
main() { x123=21; y456=x123*2; return y456; } => 42
main() { return 42; } => 42
main() { return 5; return 8; } => 5
main() { a=3; b=5*6-8; return a+b/2; } => 14
main() { return 2*5; } => 10
main() { a=10; return a-3; } => 7
main() { return 3*5; a=99; } => 15
main() { if (1) return 42; return 0; } => 42
main() { if (0) return 42; return 0; } => 0
main() { if (2-1) return 1; return 0; } => 1
main() { if (1-1) return 1; return 0; } => 0
main() { a=5; if (a) return a; return 0; } => 5
main() { a=0; if (a) return a; return 10; } => 10
main() { if (1) return 42; else return 24; } => 42
main() { if (0) return 42; else return 24; } => 24
main() { if (1==1) return 10; else return 20; } => 10
main() { if (1==2) return 10; else return 20; } => 20
main() { a=3; if (a>5) return 1; else return a; } => 3
main() { a=3; b=4; if (a>b) return a; else return a+b; } => 7
main() { i=0; while (i<10) i=i+1; return i; } => 10
main() { i=0; while (i<3) i=i+1; return i; } => 3
main() { i=0; while (i<0) i=i+1; return i; } => 0
main() { i=1; while (i<10) i=i*2; return i; } => 16
main() { for (i=0; i<5; i=i+1) j=i; return j; } => 4
main() { for (i=0; i<=10; i=i+1) ; return i; } => 11
main() { sum=0; for (i=1; i<=9; i=i+1) sum=sum+i; return sum; } => 45
main() { j=99; for (i=0; i<0; i=i+1) j=i; return j; } => 99
main() { i=0; for (; i<5; i=i+1) ; return i; } => 5
main() { for (i=0; ; i=i+1) if (i>=10) return i; } => 10
main() { for (i=1; i<=3; i=i+1) if (i==3) return i*2; return 0; } => 6
main() { i=1; while (i<5) if (i==4) return i; else i=i+1; return 0; } => 4
main() { if (1) for (i=0; i<10; i=i+1) j=i+1; else j=0; return j; } => 10
main() { if (3>2) if (1<2) return 1; return 0; } => 1
main() { for (i=0; i<3; i=i+1) for (j=0; j<2; j=j+1) k=i*2+j; return k; } => 5
main() { a=3; b=5; if (a<b) return a*b; else return a+b; } => 15
main() { n=4; sum=0; for (i=1; i<=n; i=i+1) sum=sum+i*i; return sum; } => 30
main() { x=2; result=1; for (i=0; i<x; i=i+1) result=result*2; return result; } => 4
func() { return 42; } main() { return func(); } => 42
func() { return 0; } main() { return func(); } => 0
getValue() { return 5; } main() { return getValue(); } => 5
add(x, y) { return x + y; } main() { return add(3, 4); } => 7
mul(a, b) { return a * b; } main() { return mul(2, 3); } => 6
sub(x, y) { return x - y; } main() { return sub(5, 4); } => 1
div(x, y) { return x / y; } main() { return div(10, 5); } => 2
sum3(a, b, c) { return a + b + c; } main() { return sum3(7, 8, 9); } => 24
product3(x, y, z) { return x * y * z; } main() { return product3(3, 4, 5); } => 60
double(x) { return x * 2; } main() { a = 5; return double(a); } => 10
addOne(n) { return n + 1; } main() { x = 7; return addOne(x); } => 8
add(x, y) { return x + y; } mul(a, b) { return a * b; } main() { return add(mul(2, 3), mul(3, 3)); } => 15
square(x) { return x * x; } add(a, b) { return a + b; } main() { return add(square(2), square(3)) - 3; } => 10
max(x, y) { if (x > y) return x; else return y; } main() { return max(2, 3); } => 3
max(x, y) { if (x > y) return x; else return y; } main() { return max(5, 3); } => 5
factorial(n) { result = 1; for (i = 1; i <= n; i = i + 1) result = result * i; return result; } main() { return factorial(3); } => 6
fib(n) { if (n <= 1) return n; return fib(n-1) + fib(n-2); } main() { return fib(5); } => 5
fact(n) { if (n <= 1) return 1; return n * fact(n-1); } main() { return fact(5); } => 120
sum(n) { if (n <= 0) return 0; return n + sum(n-1); } main() { return sum(10); } => 55
All tests succeeded 🎉
```
</details>

## Cleaning
Delete all temporary files with the following command.
```shell
make clean
```

## References
- [chibicc](https://github.com/rui314/chibicc)
- [8cc](https://github.com/rui314/8cc)
- [minigo](https://github.com/d0iasm/minigo)
- [compilium](https://github.com/hikalium/compilium)
- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
