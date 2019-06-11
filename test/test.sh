#!/bin/bash
try() {
    expected="$1"
    input="$2"

    dist/ccpr "$input" > tmp/tmp.s
    gcc -o tmp/tmp tmp/tmp.s
    tmp/tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else    
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 0
try 42 42
try 21 '5+20-4'
try 2 '5 - 3'
try 41 " 12 + 34 - 5 "
try 41 "12+34 - 5"
try 52 "2 + (10 * 5)"
try 3 "5 - 8 / 4"
try 47 "5+6*7"
try 15 "5*(9-6)"
try 4 "(3+5)/2"
try 5 "-10 + 15"
try 38 "50 + -12"

echo OK