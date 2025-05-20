assert() {
  expected="$1"
  input="$2"

  ./1cc "$input" > tmp.s
  cc -o tmp tmp.s -Wa,--generate-missing-build-notes=yes -z noexecstack
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
assert 0 0
assert 42 42

# Additional tests for more complex expressions
assert 15 "3 * (2 + 3)"
assert 4 "8 / (1 + 1)"
assert 10 "(7 + 3) * (9 - 8) / 1"
assert 10 "10 * (1 + 2) / 3"

# Tests for operator precedence
assert 11 "1 + 2 * 3 + 4"
assert 14 "2 * 3 + 2 * 4"
assert 10 "24 / 4 + 4"
assert 6 "1 + 10 / 2"

# Tests for nested parentheses
assert 21 "3 * (2 + (1 + 4))"
assert 8 "(2 + 2) * (3 - 1)"
assert 36 "(2 + 4) * (1 + 5)"
assert 3 "((((3)))))"

# Tests for unary operators in complex expressions
assert 6 "3 * (+3 - 1)"
assert 7 "-3 + (2 * 5)"
assert 5 "10 / +2"
assert 15 "-3 * (-4 + -1)"

# Tests for complex expressions with whitespace
assert 55 "  1 +  2+ 3 +4+5+   6 + 7   +8+9  +  10"
assert 100 "  (   5   +   5  )  *  (   5  +   5   ) "

# Tests for comparators
assert 1 '0<1'
assert 0 '1<1'
assert 0 '2<1'
assert 1 '0<=1'
assert 1 '1<=1'
assert 0 '2<=1'
assert 1 '1>0'
assert 0 '1>1'
assert 0 '1>2'
assert 1 '1>=0'
assert 1 '1>=1'
assert 0 '1>=2'

echo "All tests succeeded 🎉"
