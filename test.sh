assert() {
  expected="$1"
  input="$2"

  ./1cc "$input" > tmp.s
  gcc -o tmp tmp.s -Wa,--generate-missing-build-notes=yes -z noexecstack
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
assert 0 "0;"
assert 42 "42;"

# Additional tests for more complex expressions
assert 15 "3 * (2 + 3);"
assert 4 "8 / (1 + 1);"
assert 10 "(7 + 3) * (9 - 8) / 1;"
assert 10 "10 * (1 + 2) / 3;"

# Tests for operator precedence
assert 11 "1 + 2 * 3 + 4;"
assert 14 "2 * 3 + 2 * 4;"
assert 10 "24 / 4 + 4;"
assert 6 "1 + 10 / 2;"

# Tests for nested parentheses
assert 21 "3 * (2 + (1 + 4));"
assert 8 "(2 + 2) * (3 - 1);"
assert 36 "(2 + 4) * (1 + 5);"
assert 3 "((((3))));"

# Tests for unary operators in complex expressions
assert 6 "3 * (+3 - 1);"
assert 7 "-3 + (2 * 5);"
assert 5 "10 / +2;"
assert 15 "-3 * (-4 + -1);"

# Tests for complex expressions with whitespace
assert 55 "  1 +  2+ 3 +4+5+   6 + 7   +8+9  +  10;"
assert 100 "  (   5   +   5  )  *  (   5  +   5   ) ;"

# Tests for equality
assert 0 "0==1;"
assert 1 "42==42;"
assert 1 "0!=1;"
assert 0 "42!=42;"

# Tests for comparators
assert 1 "0<1;"
assert 0 "1<1;"
assert 0 "2<1;"
assert 1 "0<=1;"
assert 1 "1<=1;"
assert 0 "2<=1;"
assert 1 "1>0;"
assert 0 "1>1;"
assert 0 "1>2;"
assert 1 "1>=0;"
assert 1 "1>=1;"
assert 0 "1>=2;"

# Tests for assignments
assert 3 "a=3; a;"
assert 8 "a=3; z=5; a+z;"
assert 6 "a=b=3; a+b;"
assert 15 "a=3*5;"
assert 5 "a=4*8*5;b=16*2;a/b;"

# Tests for multi-character variables
assert 6 "foo=1; bar=2+3; foo+bar;"
assert 5 "abc=10; xyz=5; abc-xyz;"
assert 6 "foo=3; foo=foo*2; foo;"
assert 21 "hello=7; world=hello*2; hello+world;"
assert 100 "variable1=25; variable2=variable1*4; variable2;"
assert 42 "x123=21; y456=x123*2; y456;"

# Tests for return statements
assert 42 "return 42;"
assert 5 "return 5; return 8;"
assert 14 "a=3; b=5*6-8; return a+b/2;"
assert 10 "return 2*5;"
assert 7 "a=10; return a-3;"
assert 15 "return 3*5; a=99;"

# Tests for if statements
assert 42 "if (1) return 42; return 0;"
assert 0 "if (0) return 42; return 0;"
assert 1 "if (2-1) return 1; return 0;"
assert 0 "if (1-1) return 1; return 0;"
assert 5 "a=5; if (a) return a; return 0;"
assert 10 "a=0; if (a) return a; return 10;"

# Tests for if-else statements
assert 42 "if (1) return 42; else return 24;"
assert 24 "if (0) return 42; else return 24;"
assert 10 "if (1==1) return 10; else return 20;"
assert 20 "if (1==2) return 10; else return 20;"
assert 3 "a=3; if (a>5) return 1; else return a;"
assert 7 "a=3; b=4; if (a>b) return a; else return a+b;"

# Tests for while loops
assert 10 "i=0; while (i<10) i=i+1; return i;"
assert 3 "i=0; while (i<3) i=i+1; return i;"
assert 0 "i=0; while (i<0) i=i+1; return i;"
assert 16 "i=1; while (i<10) i=i*2; return i;"

# Tests for for loops
assert 4 "for (i=0; i<5; i=i+1) j=i; return j;"
assert 11 "for (i=0; i<=10; i=i+1) ; return i;"
assert 45 "sum=0; for (i=1; i<=9; i=i+1) sum=sum+i; return sum;"

# Tests for for loops with empty clauses
assert 99 "j=99; for (i=0; i<0; i=i+1) j=i; return j;"
assert 5 "i=0; for (; i<5; i=i+1) ; return i;"
assert 10 "for (i=0; ; i=i+1) if (i>=10) return i;"

# Tests for nested control structures
assert 6 "for (i=1; i<=3; i=i+1) if (i==3) return i*2; return 0;"
assert 4 "i=1; while (i<5) if (i==4) return i; else i=i+1; return 0;"
assert 10 "if (1) for (i=0; i<10; i=i+1) j=i+1; else j=0; return j;"
assert 1 "if (3>2) if (1<2) return 1; return 0;"
assert 5 "for (i=0; i<3; i=i+1) for (j=0; j<2; j=j+1) k=i*2+j; return k;"

# Tests for control structures with variables
assert 15 "a=3; b=5; if (a<b) return a*b; else return a+b;"
assert 30 "n=4; sum=0; for (i=1; i<=n; i=i+1) sum=sum+i*i; return sum;"
assert 4 "x=2; result=1; for (i=0; i<x; i=i+1) result=result*2; return result;"

echo "All tests succeeded 🎉"
