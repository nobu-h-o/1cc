# 1cc
1cc is a toy compiler for C.

## Progress
- [x] Basic Math
- [x] Tokenizer
- [ ] Lexical analysis
- [ ] Parser

## Compiling and executing
Use the makefile to compile 1cc to bytecode.
```shell
make
```
Next, you can execute the bytecode and input its output to a temporary file. We'll use simple arithmetic as an example.
```shell
./1cc "-3*(3+2*-4)" > tmp.s
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
  push 0
  push 3
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 3
  push 2
  push 0
  push 4
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rax
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
0 => 0
42 => 42
3 * (2 + 3) => 15
8 / (1 + 1) => 4
(7 + 3) * (9 - 8) / 1 => 10
10 * (1 + 2) / 3 => 10
1 + 2 * 3 + 4 => 11
2 * 3 + 2 * 4 => 14
24 / 4 + 4 => 10
1 + 10 / 2 => 6
3 * (2 + (1 + 4)) => 21
(2 + 2) * (3 - 1) => 8
(2 + 4) * (1 + 5) => 36
((((3))))) => 3
3 * (+3 - 1) => 6
-3 + (2 * 5) => 7
10 / +2 => 5
-3 * (-4 + -1) => 15
  1 +  2+ 3 +4+5+   6 + 7   +8+9  +  10 => 55
  (   5   +   5  )  *  (   5  +   5   )  => 100
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
