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
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 0
assert 42 42

assert 21 "5+20-4"
assert 120 "9+5+15+21-12+41-0+41"

assert 21 "5 + 20 - 4"
assert 120 "9 + 5 + 15 + 21 - 12 + 41 - 0 + 41"

assert 47 "5+6*7"
assert 4 "(3+5)/2"
assert 5 "25 / 5"
echo OK
