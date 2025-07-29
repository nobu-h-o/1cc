assert() {
  expected="$1"
  input="$2"

  ./1cc "$input" > tmp.s
  gcc -no-pie -o tmp tmp.s -Wa,--generate-missing-build-notes=yes -z noexecstack
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $actual received, but expected $expected ❌"
    exit 1
  fi
}

# Recognition
assert 0 "main() { return 0; }"
assert 42 "main() { return 42; }"

# Additional tests for more complex expressions
assert 15 "main() { return 3 * (2 + 3); }"
assert 4 "main() { return 8 / (1 + 1); }"
assert 10 "main() { return (7 + 3) * (9 - 8) / 1; }"
assert 10 "main() { return 10 * (1 + 2) / 3; }"

# Tests for operator precedence
assert 11 "main() { return 1 + 2 * 3 + 4; }"
assert 14 "main() { return 2 * 3 + 2 * 4; }"
assert 10 "main() { return 24 / 4 + 4; }"
assert 6 "main() { return 1 + 10 / 2; }"

# Tests for nested parentheses
assert 21 "main() { return 3 * (2 + (1 + 4)); }"
assert 8 "main() { return (2 + 2) * (3 - 1); }"
assert 36 "main() { return (2 + 4) * (1 + 5); }"
assert 3 "main() { return ((((3)))); }"

# Tests for unary operators in complex expressions
assert 6 "main() { return 3 * (+3 - 1); }"
assert 7 "main() { return -3 + (2 * 5); }"
assert 5 "main() { return 10 / +2; }"
assert 15 "main() { return -3 * (-4 + -1); }"

# Tests for complex expressions with whitespace
assert 55 "main() { return   1 +  2+ 3 +4+5+   6 + 7   +8+9  +  10; }"
assert 100 "main() { return   (   5   +   5  )  *  (   5  +   5   ); }"

# Tests for equality
assert 0 "main() { return 0==1; }"
assert 1 "main() { return 42==42; }"
assert 1 "main() { return 0!=1; }"
assert 0 "main() { return 42!=42; }"

# Tests for comparators
assert 1 "main() { return 0<1; }"
assert 0 "main() { return 1<1; }"
assert 0 "main() { return 2<1; }"
assert 1 "main() { return 0<=1; }"
assert 1 "main() { return 1<=1; }"
assert 0 "main() { return 2<=1; }"
assert 1 "main() { return 1>0; }"
assert 0 "main() { return 1>1; }"
assert 0 "main() { return 1>2; }"
assert 1 "main() { return 1>=0; }"
assert 1 "main() { return 1>=1; }"
assert 0 "main() { return 1>=2; }"

# Tests for assignments
assert 3 "main() { a=3; return a; }"
assert 8 "main() { a=3; z=5; return a+z; }"
assert 6 "main() { a=b=3; return a+b; }"
assert 15 "main() { a=3*5; return a; }"
assert 5 "main() { a=4*8*5; b=16*2; return a/b; }"

# Tests for multi-character variables
assert 6 "main() { foo=1; bar=2+3; return foo+bar; }"
assert 5 "main() { abc=10; xyz=5; return abc-xyz; }"
assert 6 "main() { foo=3; foo=foo*2; return foo; }"
assert 21 "main() { hello=7; world=hello*2; return hello+world; }"
assert 100 "main() { variable1=25; variable2=variable1*4; return variable2; }"
assert 42 "main() { x123=21; y456=x123*2; return y456; }"

# Tests for return statements
assert 42 "main() { return 42; }"
assert 5 "main() { return 5; return 8; }"
assert 14 "main() { a=3; b=5*6-8; return a+b/2; }"
assert 10 "main() { return 2*5; }"
assert 7 "main() { a=10; return a-3; }"
assert 15 "main() { return 3*5; a=99; }"

# Tests for if statements
assert 42 "main() { if (1) return 42; return 0; }"
assert 0 "main() { if (0) return 42; return 0; }"
assert 1 "main() { if (2-1) return 1; return 0; }"
assert 0 "main() { if (1-1) return 1; return 0; }"
assert 5 "main() { a=5; if (a) return a; return 0; }"
assert 10 "main() { a=0; if (a) return a; return 10; }"

# Tests for if-else statements
assert 42 "main() { if (1) return 42; else return 24; }"
assert 24 "main() { if (0) return 42; else return 24; }"
assert 10 "main() { if (1==1) return 10; else return 20; }"
assert 20 "main() { if (1==2) return 10; else return 20; }"
assert 3 "main() { a=3; if (a>5) return 1; else return a; }"
assert 7 "main() { a=3; b=4; if (a>b) return a; else return a+b; }"

# Tests for while loops
assert 10 "main() { i=0; while (i<10) i=i+1; return i; }"
assert 3 "main() { i=0; while (i<3) i=i+1; return i; }"
assert 0 "main() { i=0; while (i<0) i=i+1; return i; }"
assert 16 "main() { i=1; while (i<10) i=i*2; return i; }"

# Tests for for loops
assert 4 "main() { for (i=0; i<5; i=i+1) j=i; return j; }"
assert 11 "main() { for (i=0; i<=10; i=i+1) ; return i; }"
assert 45 "main() { sum=0; for (i=1; i<=9; i=i+1) sum=sum+i; return sum; }"

# Tests for for loops with empty clauses
assert 99 "main() { j=99; for (i=0; i<0; i=i+1) j=i; return j; }"
assert 5 "main() { i=0; for (; i<5; i=i+1) ; return i; }"
assert 10 "main() { for (i=0; ; i=i+1) if (i>=10) return i; }"

# Tests for nested control structures
assert 6 "main() { for (i=1; i<=3; i=i+1) if (i==3) return i*2; return 0; }"
assert 4 "main() { i=1; while (i<5) if (i==4) return i; else i=i+1; return 0; }"
assert 10 "main() { if (1) for (i=0; i<10; i=i+1) j=i+1; else j=0; return j; }"
assert 1 "main() { if (3>2) if (1<2) return 1; return 0; }"
assert 5 "main() { for (i=0; i<3; i=i+1) for (j=0; j<2; j=j+1) k=i*2+j; return k; }"

# Tests for control structures with variables
assert 15 "main() { a=3; b=5; if (a<b) return a*b; else return a+b; }"
assert 30 "main() { n=4; sum=0; for (i=1; i<=n; i=i+1) sum=sum+i*i; return sum; }"
assert 4 "main() { x=2; result=1; for (i=0; i<x; i=i+1) result=result*2; return result; }"

# Tests for basic functions
assert 42 "func() { return 42; } main() { return func(); }"
assert 0 "func() { return 0; } main() { return func(); }"
assert 5 "getValue() { return 5; } main() { return getValue(); }"

# Tests for functions with parameters
assert 7 "add(x, y) { return x + y; } main() { return add(3, 4); }"
assert 6 "mul(a, b) { return a * b; } main() { return mul(2, 3); }"
assert 1 "sub(x, y) { return x - y; } main() { return sub(5, 4); }"
assert 2 "div(x, y) { return x / y; } main() { return div(10, 5); }"

# Tests for functions with multiple parameters
assert 24 "sum3(a, b, c) { return a + b + c; } main() { return sum3(7, 8, 9); }"
assert 60 "product3(x, y, z) { return x * y * z; } main() { return product3(3, 4, 5); }"

# Tests for functions with variables
assert 10 "double(x) { return x * 2; } main() { a = 5; return double(a); }"
assert 8 "addOne(n) { return n + 1; } main() { x = 7; return addOne(x); }"

# Tests for nested function calls
assert 15 "add(x, y) { return x + y; } mul(a, b) { return a * b; } main() { return add(mul(2, 3), mul(3, 3)); }"
assert 10 "square(x) { return x * x; } add(a, b) { return a + b; } main() { return add(square(2), square(3)) - 3; }"

# Tests for functions with control flow
assert 3 "max(x, y) { if (x > y) return x; else return y; } main() { return max(2, 3); }"
assert 5 "max(x, y) { if (x > y) return x; else return y; } main() { return max(5, 3); }"
assert 6 "factorial(n) { result = 1; for (i = 1; i <= n; i = i + 1) result = result * i; return result; } main() { return factorial(3); }"

# Tests for recursive functions  
assert 5 "fib(n) { if (n <= 1) return n; return fib(n-1) + fib(n-2); } main() { return fib(5); }"
assert 120 "fact(n) { if (n <= 1) return 1; return n * fact(n-1); } main() { return fact(5); }"
assert 55 "sum(n) { if (n <= 0) return 0; return n + sum(n-1); } main() { return sum(10); }"

echo "All tests succeeded 🎉"
